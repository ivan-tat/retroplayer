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
    sdev_mode_rate: word;
    sdev_mode_16bits: boolean;
    sdev_mode_signed: boolean;
    sdev_mode_stereo: boolean;

procedure sb_set_volume(value: Byte);
procedure sbAdjustMode( var rate: word; var stereo: boolean; var _16bits: boolean );
procedure sbSetupMode( freq: word; stereo: boolean );
procedure sbSetupDMATransfer( p: pointer; count: word; autoinit: boolean );
function  sbGetDMACounter: word;
procedure sbSetupDSPTransfer( len: word; auto: boolean );
procedure Initblaster(var m16bits, mStereo: boolean; var mRate: word);
procedure pause_play;
procedure continue_play;
procedure stop_play;
procedure set_ready_irq(p:pointer);
procedure restore_irq;

procedure Forceto(typ, dma8, dma16, irq: Byte; dsp: Word);
function  Detect_DSP_Addr: Boolean;
function  Detect_DMA_Channel_IRQ: Boolean;
function  DetectSoundblaster: Boolean;
function  UseBlasterEnv: Boolean;
function  InputSoundblasterValues: Boolean;
procedure writelnSBConfig;

implementation

uses
    pascal,
    watcom,
    strutils,
    i86,
    stdio,
    conio,
    stdlib,
    string_,
    errno_,
    dos,
    debug,
    dma,
    pic,
    sbio,
    detisr,
    sndisr,
    crt;

(*$l sbctl.obj*)

procedure sb_set_volume(value: Byte); external;
procedure sbAdjustMode( var rate: word; var stereo: boolean; var _16bits: boolean ); external;
procedure sbSetupMode( freq: word; stereo: boolean ); external;
procedure sbSetupDMATransfer( p: pointer; count: word; autoinit: boolean ); external;
function  sbGetDMACounter: word; external;
procedure sbSetupDSPTransfer( len: word; auto: boolean ); external;
procedure Initblaster(var m16bits, mStereo: boolean; var mRate: word); external;
procedure pause_play; external;
procedure continue_play; external;
procedure stop_play; external;
procedure set_ready_irq(p: Pointer); external;
procedure restore_irq; external;

procedure Forceto(typ, dma8, dma16, irq: Byte; dsp: Word); external;
function  Detect_DSP_Addr: Boolean; external;
function  Detect_DMA_Channel_IRQ: Boolean; external;
function  DetectSoundblaster: Boolean; external;
function  UseBlasterEnv: Boolean; external;
function  InputSoundblasterValues: Boolean; external;
procedure writelnSBConfig; external;

procedure register_sbctl; far; external;

begin
  register_sbctl;
end.
