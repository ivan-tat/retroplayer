(* sbctl.pas -- Pascal declarations for sbctl.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

{$A+,B-,D+,E-,F-,G+,I-,L+,N-,O-,P-,Q-,R-,S-,T-,V-,X+,Y+}
{$M 16384,0,655360}

unit sbctl;

interface

(*$I defines.pas*)

(*---------------------------------------------------------------------+
| This is a specially for detecting and using the soundblaster card.   |
| autodetect routines work for dsp adresses 210 to 280,                |
| 8bit dmachannels 0,1,3/ 16bit dmachannel 5 and for interrupts 2,5,7  |
| (known problems with IRQ2)                                           |
| -------------------------------------------------------------------- |
| no more work on autodetection - wait for craigs source, takes better |
| advantage to our knowledge about different SB models, we got in last |
| time. i.e. you can read IRQ#/DMA8/DMA16 from mixer port on SB16      |
| -------------------------------------------------------------------- |
| I read about 16Bit improvment for SB PRO3/4.0 but don't know about - |
| anybody can help me with that ?                                      |
| how to use it ... any specifications for it ?                        |
| -------------------------------------------------------------------- |
| STATUS: TESTED ON SB16/ASP,SB PRO2,SB2.0                             |
|         COMMENTs READY                                               |
|   KNOWN PROBLEMS: - IRQ2 detection is wrong on some maschines        |
|                   - no 16bit IRQ autodetection :(                    |
|                   - no check if values are right in                  |
|                              * UseBlasterEnv                         |
|                              * InputBlasterValues                    |
|                   - no mixedup detection (i.e. some values as        |
|                     parameters and detect the rest)                  |
|                     I though about this while I had a SB PRO and a   |
|                     SB16 at the same time 'inside' on base 220h,240h |
+---------------------------------------------------------------------*)

(* The different SoundBlaster versions and its hardware :

   #  Name                   Mono    Stereo   Bits   Mixer   DSP
   0. None                       -        -      -       -      -
   1. Soundblaster 1.0
      Soundblaster 1.5       23kHz        -      8      no   1.xx

   2. Soundblaster Pro       45kHz    23kHz      8     yes   3.xx

   3. Soundblaster 2.0                                 no?
      Audioblaster 2.5       45kHz        -      8     yes   2.xx

   4. Soundblaster Pro 3
      Audioblaster Pro 4.0   45kHz    23kHz      8     yes   3.xx

   5. Soundblaster Pro (Mircochannel)
        Special version for PS/2 - technical data = 4

   6. Soundblaster 16
      Soundblaster 16 ASP    45kHz    45kHz    8,16    yes   4.xx

   DSP versions :

   SoundBlaster 1.0/1.5        1.xx
   SoundBlaster 2.0/2.5        2.xx
   SoundBlaster Pro/PRO3/PRO4  3.xx
   SoundBlaster 16/ASP         4.xx
*)

var
    sdev_type: Byte;
    sdev_name: PChar;
    sdev_hw_base: word;
    sdev_hw_irq: byte;
    sdev_hw_dma8: byte;
    sdev_hw_dma16: byte;

    sdev_caps_mixer: boolean;
    sdev_caps_stereo: boolean;
    sdev_caps_16bit: boolean;
    sdev_caps_mono_maxrate: word;
    sdev_caps_stereo_maxrate: word;

    sdev_mode_rate: word;
    sdev_mode_16bits: boolean;
    sdev_mode_signed: boolean;
    sdev_mode_stereo: boolean;

procedure setvolume( vol: byte );
function  sbReadDSPVersion: word;
procedure sbAdjustMode( var rate: word; var stereo: boolean; var _16bit: boolean );
procedure sbSetupMode( freq: word; stereo: boolean );
procedure sbSetupDMATransfer( p: pointer; count: word; autoinit: boolean );
function  sbGetDMACounter: word;
procedure sbSetupDSPTransfer( len: word; auto: boolean );
procedure pause_play;
procedure continue_play;
procedure stop_play;

procedure Forceto(typ, dma8, dma16, irq: Byte; dsp: Word);
function  UseBlasterEnv:boolean;                        (* use values set in enviroment BLASTER *)
function  InputSoundblasterValues:Boolean;              (* input Soundbaster values by hand (in textmode) *)
function  Detect_DSP_Addr: Boolean;
function  Detect_DMA_Channel_IRQ: Boolean;
function  DetectSoundblaster: Boolean;
procedure Initblaster(var m16bits, mStereo: boolean; var mRate: word);
procedure set_ready_irq(p:pointer);
procedure restore_irq;
procedure writelnSBConfig;

implementation

uses
    pascal,
    watcom,
    strutils,
    i86,
    stdio,
    conio,
    dos,
    debug,
    dma,
    pic,
    sbio,
    detisr,
    sndisr,
    crt;

const
    __FILE__ = 'sbctl.pas';

(*$l sbctl.obj*)

procedure setvolume( vol: byte ); external;
function  sbReadDSPVersion: word; external;
procedure sbAdjustMode( var rate: word; var stereo: boolean; var _16bit: boolean ); external;
procedure sbSetupMode( freq: word; stereo: boolean ); external;
procedure sbSetupDMATransfer( p: pointer; count: word; autoinit: boolean ); external;
function  sbGetDMACounter: word; external;
procedure sbSetupDSPTransfer( len: word; auto: boolean ); external;
procedure Initblaster(var m16bits, mStereo: boolean; var mRate: word); external;
procedure pause_play; external;
procedure continue_play; external;
procedure stop_play; external;

(* Flags and variables for detect part *)
var
    sdev_configured: boolean;    (* sound card is detected *)
    sdev_hwflags_base: boolean;  (* base i/o address is detected *)
    sdev_hwflags_irq: boolean;   (* IRQ is detected *)
    sdev_hwflags_dma8: boolean;  (* DMA 8-bit channel is detected *)
    sdev_hwflags_dma16: boolean; (* DMA 8-bit channel is detected *)
    sdev_hw_dspv: word;          (* DSP chip version *)
    sdev_irq_answer: byte;       (* for detecting *)
    sdev_irq_savedvec: pointer;  (*  "       "    *)

(* ISR *)
var
    ISRUserCallback: PSoundHWISRCallback;

procedure ISRDetectCallback(irq: Byte); far; external;
procedure ISRSoundPlayback; far; external;

procedure _sb_set_hw_dsp(typ: Byte; dspv: Word); far; external;
procedure _sb_set_hw_flags(f_base, f_irq, f_dma8, f_dma16: Boolean); far; external;
procedure _sb_set_hw_config(base: Word; irq, dma8, dma16: Byte); far; external;
procedure _sb_set_mode(rate: Word; f_16bits, f_signed, f_stereo: Boolean); far; external;

procedure Forceto(typ, dma8, dma16, irq: Byte; dsp: Word); external;
function  Detect_DSP_Addr: Boolean; external;
function  Detect_DMA_Channel_IRQ: Boolean; external;
function  DetectSoundblaster: Boolean; external;
procedure set_ready_irq(p: Pointer); external;
procedure restore_irq; external;
procedure writelnSBConfig; external;

type
    TSBCFGFLAGS = Byte;

const
    SBCFGFL_TYPE  = 1 shl 0;
    SBCFGFL_DSP   = 1 shl 1;
    SBCFGFL_IRQ   = 1 shl 2;
    SBCFGFL_DMA8  = 1 shl 3;
    SBCFGFL_DMA16 = 1 shl 4;
    SBCFGFL_BASE_MASK = SBCFGFL_TYPE or SBCFGFL_DSP or SBCFGFL_IRQ or SBCFGFL_DMA8;

function _check_value_type(v: Byte): Boolean; near; external;
function _check_value_dsp(v: Word): Boolean; near; external;
function _check_value_irq(v: Byte): Boolean; near; external;
function _check_value_dma(v: Byte): Boolean; near; external;

function UseBlasterEnv:boolean;
var
    s, u: String;
    typ,irq,dma8,dma16: Byte;
    dsp: Word;
    count, i: Byte;
    er: Integer;
    flags: TSBCFGFLAGS;
    v: Word;
begin
    DEBUG_BEGIN(__FILE__, 'UseBlasterEnv');

    _sb_set_hw_dsp(0, 0);
    _sb_set_hw_flags(false, false, false, false);
    _sb_set_hw_config($ffff, $ff, $ff, $ff);
    _sb_set_mode(0, false, false, false);

    s := upstr(getenv('BLASTER'));
    if s='' then
    begin
        UseBlasterEnv := false;
        exit;
    end;

    flags := 0;

    (* BLASTER=A220 I? D? H? P??? T? *)

    i := pos('T', s);
    if (i > 0) then
    begin
        u := copy(s, i+1, 1);
        val(u, v, er);
        if ((er = 0)  and _check_value_type(v)) then
        begin
            typ := v;
            flags := flags or SBCFGFL_TYPE;
        end;
    end;

    i := pos('A', s);
    if (i > 0) then
    begin
        u := copy(s, i+1, 3);
        val(u, v, er);
        v := (v div 100) * 256 + ((v div 10) mod 10) * 16 + (v mod 10);
        if ((er = 0) and _check_value_dsp(v)) then
        begin
            dsp := v;
            flags := flags or SBCFGFL_DSP;
        end;
    end;

    i := pos('I',s);
    if (i > 0) then
    begin
        if (s[i + 2] <> ' ') then
            u := copy(s, i+1, 2)
        else
            u := copy(s, i+1, 1);
        val(u, v, er);
        if ((er = 0) and _check_value_irq(v)) then
        begin
            irq := v;
            flags := flags or SBCFGFL_IRQ;
        end;
    end;

    i := pos('D', s);
    if (i > 0) then
    begin
        u := copy(s, i+1, 1);
        val(u, v, er);
        if ((er = 0) and _check_value_dma(v)) then
        begin
            dma8 := v;
            flags := flags or SBCFGFL_DMA8;
        end;
    end;

    i := pos('H', s);
    if (i > 0) then
    begin
        u := copy(s, i+1, 1);
        val(u, v, er);
        if ((er = 0) and _check_value_dma(v)) then
        begin
            dma16 := v;
            flags := flags or SBCFGFL_DMA16;
        end;
    end;

    if (flags and SBCFGFL_BASE_MASK = SBCFGFL_BASE_MASK) then
    begin
        Forceto(typ, dma8, dma16, irq, dsp);
    end
    else
    begin
        DEBUG_FAIL(__FILE__, 'UseBlasterEnv', 'Configuration string is not complete.');
        UseBlasterEnv := false;
        exit;
    end;

    DEBUG_SUCCESS(__FILE__, 'UseBlasterEnv');
    UseBlasterEnv := true;
end;

function  InputSoundblasterValues: Boolean; external;

procedure register_sbctl; far; external;

begin
  register_sbctl;
end.
