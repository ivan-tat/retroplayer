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
    sdev_name: PChar;
    sdev_hw_base: word;
    sdev_hw_irq: byte;
    sdev_hw_dma8: byte;
    sdev_hw_dma16: byte;

    sbno: byte;                 (* SoundBlaster typ (look some rows above) *)

    sdev_caps_mixer: boolean;
    sdev_caps_stereo: boolean;
    sdev_caps_16bit: boolean;
    sdev_caps_mono_maxrate: word;
    sdev_caps_stereo_maxrate: word;

    sdev_mode_rate: word;
    sdev_mode_16bit: boolean;
    sdev_mode_signed: boolean;
    sdev_mode_stereo: boolean;

procedure setvolume( vol: byte );
function sbReadDSPVersion: word;
procedure sbAdjustMode( var rate: word; var stereo: boolean; var _16bit: boolean );
procedure sbSetupMode( freq: word; stereo: boolean );
procedure sbSetupDMATransfer( p: pointer; count: word; autoinit: boolean );
function sbGetDMACounter: word;
procedure sbSetupDSPTransfer( len: word; auto: boolean );
procedure pause_play;
procedure continue_play;
procedure stop_play;

procedure Forceto(typ,dma,dma16,irq:byte;dsp:word);     (* force to use these values for playing *)
function  UseBlasterEnv:boolean;                        (* use values set in enviroment BLASTER *)
function  InputSoundblasterValues:Boolean;              (* input Soundbaster values by hand (in textmode) *)
function  DetectSoundblaster(prot:boolean):Boolean;     (* detects Sound Blaster card *)
function  Detect_DSP_Addr(prot:boolean):Boolean;        (* detects DSP *)
function  Detect_DMA_Channel_IRQ(prot:boolean):Boolean; (* detects DMA & IRQ channels *)
function  Get_BlasterVersion:Word;                      (* reads the detected soundblaster version *)
procedure sb_adjust_mode(var f_16bits, f_stereo: boolean; var rate: word);
procedure Initblaster(var m16bits, mStereo: boolean; var mRate: word);
procedure set_ready_irq(p:pointer);
procedure restore_irq;
procedure writelnSBConfig;
procedure sbInit;
procedure sbDone;

implementation

uses
    pascal,
    watcom,
    strutils,
    i86,
    dos,
    dma,
    pic,
    sbio,
    detisr,
    sndisr,
    crt;

(*$l sbctl.obj*)

procedure setvolume( vol: byte ); external;
function sbReadDSPVersion: word; external;
procedure sbAdjustMode( var rate: word; var stereo: boolean; var _16bit: boolean ); external;
procedure sbSetupMode( freq: word; stereo: boolean ); external;
procedure sbSetupDMATransfer( p: pointer; count: word; autoinit: boolean ); external;
function sbGetDMACounter: word; external;
procedure sbSetupDSPTransfer( len: word; auto: boolean ); external;
procedure pause_play; external;
procedure continue_play; external;
procedure stop_play; external;

const
    HW_BASE_MAX = 8;
    HW_BASE_NUM: array[0..HW_BASE_MAX-1] of word = (
        $210, $220, $230, $240, $250, $260, $270, $280
    );
    HW_IRQ_MAX = 5;
    HW_IRQ_NUM: array[0..HW_IRQ_MAX-1] of byte = ( 2, 3, 5, 7, 10 );
    HW_DMA_MAX = 5;
    HW_DMA_NUM: array[0..HW_DMA_MAX-1] of byte = ( 0, 1, 3, 5, 7 );

(* Flags and variables for detect part *)
var
    sdev_configured: boolean;    (* sound card is detected *)
    sdev_hwflags_base: boolean;  (* base i/o address is detected *)
    sdev_hwflags_irq: boolean;   (* IRQ is detected *)
    sdev_hwflags_dma8: boolean;  (* DMA 8-bit channel is detected *)
    sdev_hwflags_dma16: boolean; (* DMA 8-bit channel is detected *)
    sdev_hw_dspv: word;          (* DSP chip version *)

    check: byte;       (* for detecting *)
    savvect: pointer;  (*  "       "    *)

(* ISR *)
var
    ISRUserCallback: PSoundHWISRCallback;

procedure set_hw( name: pchar; _type: byte; mixer, _16bit, stereo: boolean; m_max, s_max: word );
begin
    sdev_name := name;
    sbno := _type;
    sdev_caps_mixer := mixer;
    sdev_caps_16bit := _16bit;
    sdev_caps_stereo := stereo;
    sdev_caps_mono_maxrate := m_max;
    sdev_caps_stereo_maxrate := s_max;
end;

procedure set_hw_dsp( version: byte );
begin
    case version of
      1: set_hw( 'SoundBlaster 1.0 / 1.5', 1, false, false, false, 22050, 0 );
      2: set_hw( 'SoundBlaster 2.0 / 2.5', 3, true, false, false, 44100, 0 );
      3: set_hw( 'SoundBlaster Pro', 2, true, true, false, 44100, 22700 );
      4: set_hw( 'SoundBlaster 16 / ASP', 6, true, true, true, 45454, 45454 );
    else set_hw( '', 0, false, false, false, 0, 0 );
    end;
end;

procedure set_hwflags( base, irq, dma8, dma16: boolean );
begin
    sdev_hwflags_base := base;
    sdev_hwflags_irq := irq;
    sdev_hwflags_dma8 := dma8;
    sdev_hwflags_dma16 := dma16;
end;

procedure set_hwconfig( base: word; irq, dma8, dma16: byte );
begin
    sdev_hw_base := base;
    sdev_hw_irq := irq;
    sdev_hw_dma8 := dma8;
    sdev_hw_dma16 := dma16;
end;

procedure set_mode( m_rate: word; m_16bits, m_signed, m_stereo: boolean );
begin
    sdev_mode_rate := m_rate;
    sdev_mode_16bit := m_16bits;
    sdev_mode_signed := m_signed;
    sdev_mode_stereo := m_stereo;
end;

{ -------------------- continue commenting here ---------------------- }

procedure sb_adjust_mode(var f_16bits, f_stereo: boolean; var rate: word);
begin
    sbAdjustMode(rate, f_stereo, f_16bits); (* FIXME: +signed *)
end;

PROCEDURE Initblaster(var m16bits, mStereo: boolean; var mRate: word);
begin
    (* first reset SB: *)
    sbioDSPAcknowledgeIRQ(sdev_hw_base, false);
    sbioDSPAcknowledgeIRQ(sdev_hw_base, true);
    stop_play;
    (* Now init: *)
    sbAdjustMode(mRate, mStereo, m16bits);      (* FIXME: +signed *)
    set_mode(mRate, m16bits, false, mStereo );  (* FIXME: *signed *)
    sbSetupMode(mRate, mStereo);                (* FIXME: +16bits, +signed *)
end;

(* hardware base i/o port, IRQ, DMA detection *)

procedure ISRDetectCallback( irq: byte ); far;
begin
    check := irq;
    picEOI( irq );
    sbioDSPAcknowledgeIRQ( sdev_hw_base, false );
end;

FUNCTION Detect_DSP_Addr(prot:boolean):Boolean;
var p: word;
    i: word;
begin
    if ( sdev_hwflags_base ) then
    begin
        detect_dsp_addr := true;
        exit;
    end;

    if ( prot ) then writeln('Detecting DSP base address...');

    i := 0;
    while ( ( i < HW_BASE_MAX) and not sdev_hwflags_base ) do
    begin
        p := HW_BASE_NUM[ i ];
        if ( prot ) then write( '- probing ', hexw( p ), '... ' );

        if ( sbioDSPReset( p ) ) then
        begin
            sdev_hw_base := p;
            sdev_hwflags_base := true;
        end;

        if ( prot ) then
            if ( sdev_hwflags_base ) then
                writeln(' found')
            else
                writeln(' not found');

        inc( i );
      end;

    if ( not sdev_hwflags_base ) then
    begin
        detect_dsp_addr := false;
        exit;
    end;

    if ( prot ) then writeln( 'Checking DSP version...' );

    (* Read DSP version *)
    sdev_hw_dspv := sbReadDSPVersion;
    if ( sbioError <> E_SBIO_SUCCESS ) then
    begin
        if ( prot ) then writeln('ERROR: Unable to get DSP chip version.');
        detect_dsp_addr := false;
        exit;
    end;

    if ( ( hi( sdev_hw_dspv ) < 1 ) or ( hi( sdev_hw_dspv ) > 4 ) ) then
    begin
        if ( prot ) then writeln('ERROR: Unknown DSP chip version.');
        detect_dsp_addr := false;
        exit;
    end;

    detect_dsp_addr := true;
end;

const
    _sb_silence_u8: Byte = $80;

function tryDetectIRQ( dmac: byte; m16bits: boolean ): boolean;
var
    mRate: word;
    mStereo: boolean;
begin
    check := 0;

    if ( m16bits ) then
        sdev_hw_dma16 := dmac
    else
        sdev_hw_dma8 := dmac;

    mRate := 8000;
    mStereo := false;
    dmaMaskSingleChannel( dmac );
    stop_play;
    Initblaster(m16bits, mStereo, mRate);
    sbSetupDMATransfer( @_sb_silence_u8, 1, false );
    sbSetupDSPTransfer( 1, false );
    delay( 10 );

    if ( check <> 0 ) then
    begin
        sdev_hwflags_dma8 := true;
        sdev_hw_irq := check;
        sdev_hwflags_irq := true;
        tryDetectIRQ := true;
    end else
        tryDetectIRQ := false;
end;

FUNCTION Detect_DMA_Channel_IRQ(prot:boolean):Boolean;
var
    oldv: array[0..HW_IRQ_MAX-1] of pointer;
    i: word;
    dmac: byte;
    dmamask: byte;
    irq: byte;
    irqmask: word;
begin
    if ( sdev_hwflags_dma8 ) then
    begin
        detect_DMA_Channel_irq := true;
        exit;
    end;
    if ( not sdev_hwflags_base ) then
    begin
        detect_dma_channel_irq := false;
        exit;
    end;

    if ( prot ) then writeln( 'Detecting DMA channel and IRQ ...' );

    dmamask := 0;
    for i := 0 to HW_DMA_MAX-1 do
        dmamask := dmamask or ( 1 shl HW_DMA_NUM[ i ] );

    dmaMaskChannels( dmamask );

    asm sti end;

    SetDetISRCallback( @ISRDetectCallback );

    irqmask := 0;
    for i := 0 to HW_IRQ_MAX-1 do
    begin
        irq := HW_IRQ_NUM[ i ];
        irqmask := irqmask or ( 1 shl irq );
        oldv[i] := picGetISR( irq );
        picSetISR( irq, GetDetISR( irq ));
    end;
    (* no changes for IRQ 2 *)
    irqmask := irqmask and not ( 1 shl 2 );

    picDisableChannels( irqmask );

    i := 0;
    while ( ( i < HW_DMA_MAX) and not sdev_hwflags_dma8 ) do
    begin
        dmac := HW_DMA_NUM[ i ];

        if ( prot ) then
            write( '- trying channel ', dmac, '... ' );

        tryDetectIRQ( dmac, false );

        if ( prot ) then
        begin
            if ( sdev_hwflags_dma8 and sdev_hwflags_irq ) then
                writeln( 'found with IRQ ', check )
            else
                writeln( 'not found' );
        end;

        inc( i );
    end;

    for i := 0 to HW_IRQ_MAX-1 do
        picSetISR( HW_IRQ_NUM[i], oldv[i] );

    picEnableChannels( irqmask );

    if ( not sdev_hwflags_dma8 ) then
    begin
        Detect_DMA_Channel_irq := false;
        exit;
    end;

    sbioDSPReset( sdev_hw_base );

    Detect_DMA_Channel_irq := true;
  end;

function DetectSoundblaster( prot: boolean): boolean;
begin
    sdev_configured := false;
    set_hwflags( false, false, false, false );
    set_hw_dsp( 0 );
    sdev_mode_stereo := false;
    sdev_mode_16bit := false;

    if ( not Detect_DSP_Addr( prot ) ) then
    begin
        if ( prot ) then writeln( 'ERROR: Failed to find DSP base address.' );
        DetectSoundblaster := false;
        exit;
    end;

    { for the first set SB1.0 - should work on all SBs }
    set_hw_dsp( 1 );

    stop_play;

    if ( not Detect_DMA_Channel_irq( prot ) ) then
    begin
        if ( prot ) then writeln( 'ERROR: Failed to find DMA channel and IRQ.' );
        sbno := 0;
        DetectSoundblaster := false;
        exit;
    end;

    sbioDSPReset( sdev_hw_base );

    set_hw_dsp( hi( sdev_hw_dspv ) );

    DetectSoundblaster := ( sbno <> 0 );
end;

FUNCTION Get_BlasterVersion:Word;
  begin
    Get_BlasterVersion := sdev_hw_dspv;
  end;

procedure ISRSoundPlayback; far;
begin
    asm cli end;

    (* acknowledge the interrupt on SB: *)
    sbioDSPAcknowledgeIRQ( sdev_hw_base, sdev_mode_16bit );

    (* ackknowledge PICs: *)
    picEOI( 8 );    (* secondary *)
    picEOI( 0 );    (* primary *)

    (* now new hardware interrupts are allowed *)

    if ( ISRUserCallback <> nil ) then asm call dword ptr [ISRUserCallback] end;

    asm sti end;
end;

PROCEDURE set_ready_irq(p:pointer);
begin
    ISRUserCallback := p;
    SetSoundHWISRCallback( @ISRSoundPlayback );
    savvect := picGetISR( sdev_hw_irq );
    picSetISR( sdev_hw_irq, GetSoundHWISR );
    (* no changes for IRQ2 *)
    picDisableChannels( ( 1 shl sdev_hw_irq ) and not ( 1 shl 2 ) );
end;

PROCEDURE restore_irq;
begin
    (* no changes for IRQ2 *)
    picEnableChannels( ( 1 shl sdev_hw_irq ) and not ( 1 shl 2 ) );
    picSetISR( sdev_hw_irq, savvect );
end;

FUNCTION ready:boolean;
  begin
    ready:=check>0;
  end;

PROCEDURE Forceto(typ,dma,dma16,irq:byte;dsp:word);
var
    caps_mixer: boolean;
    caps_16bit: boolean;
    caps_stereo: boolean;
begin
    sdev_configured := true;
    set_hwflags( true, true, true, false );
    set_hwconfig( dsp, irq, dma, dma16 );
    sdev_mode_stereo := false;
    sdev_mode_16bit := false;

    case typ of
        1: set_hw_dsp( 1 );
        3: set_hw_dsp( 2 );
        2,4,5:
           set_hw_dsp( 3 );
        6: set_hw_dsp( 4 );
    end;
  end;

function UseBlasterEnv:boolean;
var s:string;
    typ,dma,dma16,irq:byte;
    dsp:word;
  function upstr(s:string):string;
    var t:string;
        i:byte;
    begin
      t:='';
      for i:=1 to length(s) do
        t:=t+upcase(s[i]);
      upstr:=t;
    end;

var count,i:byte;
    u:string;
    er:integer;

  begin
    typ:=255;
    set_hwconfig( $ffff, $ff, $ff, $ff );
    { default values (totally crap), but if you get them after calling Use....}
    { you'll know that this value is not/wrong defined in the BLASTER env. }
    UseBlasterEnv:=false;
    s:=upstr(getenv('BLASTER'));
    if s='' then exit; { no chance :( }
    count:=0;
    { SET BLASTER=A220 I? D? H? P??? T? }
    i:=pos('T',s); { Soundblaster typ }
    if i>0 then
      begin
        u:=copy(s,i+1,1); { maybe for future blaster versions not right :( }
        val(u,typ,er);
        if er=0 then inc(count); { yeah we got this value ! }
      end;
    i:=pos('D',s); { DMAchannel }
    if i>0 then
      begin
        u:=copy(s,i+1,1);
        val(u,dma,er);
        if (er=0) and (dma<4) and (dma<>2) then inc(count) { yeah we got it ! }
          else dma:=255;
      end;
    i:=pos('I',s); { IRQ number }
    if i>0 then
      begin
        if s[i+2]<>' ' then u:=copy(s,i+1,2) else u:=copy(s,i+1,1);
        val(u,irq,er);
        if (er=0) and ((irq=2) or (irq=5) or (irq=7) or (irq=10)) then inc(count)
          else irq:=255;
      end;
    i:=pos('H',s); { 16Bit DMAchannel }
    if i>0 then
      begin
        u:=copy(s,i+1,1);
        val(u,dma16,er);
        if (er<>0) or (dma16<5) or (dma16>9) then dma16:=255;
        { it does not matter if there's no value }
      end;
    i:=pos('A',s); { DSPadress }
    if i>0 then
      begin
        u:=copy(s,i+1,3);
        val(u,dsp,er);
        dsp:=(dsp div 100)*256+ ((dsp div 10) mod 10)*16 + dsp mod 10;
        if (er=0) and (dsp div 256 = 2) and (((dsp mod 256) div 16) in [2,3,4,5,6,7,8]) then inc(count)
          else dsp:=$ffff;
      end;

    if count=4 then { we got all :) }
      forceto(typ,dma,dma16,irq,dsp)
    else exit; { was not enough detectable }
    UseBlasterEnv:=true;
  end;

procedure writelnSBConfig;
  begin
    writeln(#13#10' Soundblaster typ: ');
    case sbNo of
      0: writeln(' none ');
      1: writeln(' Soundblaster 1.0/1.5 (8 bit/mono-max 24kHz)');
      2: writeln(' Soundblaster Pro (8 bit/mono-max 44kHz/stereo-max 22kHz)');
      3: writeln(' Soundblaster 2.0/2.5/junior (8 bit/mono-max 44kHz)');
      4: writeln(' Soundblaster Pro3.0/PRO 4.0 (8 bit/mono-max 44kHz/stereo-max 22kHz)');
      5: writeln(' Soundblaster Pro<microchannel> (8 bit/mono-max 44kHz/stereo-max 22kHz)');
      6: writeln(' Soundblaster 16/16 ASP (8/16 bit/mono/stereo/max 45kHz)');
    end;
    writeln( #13#10' SB-Base : 2', ( sdev_hw_base div 16 ) mod 16, '0h');
    writeln( ' 8bit DMA : ', sdev_hw_dma8 );
    if ( SBNo = 6 ) then writeln(' 16bit DMA : ', sdev_hw_dma16);
    writeln( ' IRQ : ', sdev_hw_irq, #13#10 );
  end;

FUNCTION InputSoundblasterValues:Boolean;
var c:char;
  begin
    InputSoundblasterValues:=false;
    writeln(#13#10' Soundblaster typ ? ');
    writeln(' 0) none ');
    writeln(' 1) Soundblaster 1.0/1.5                   .....  8 bit mono   (max 24kHz)');
    writeln(' 2) Soundblaster 2.0/2.5/junior            .....  8 bit mono   (max 44kHz)');
    writeln(' 3) Soundblaster Pro/Pro3.0/PRO 4.0/micro  .....  8 bit stereo (max 22kHz)');
    writeln(' 4) Soundblaster 16/16 ASP                 ..... 16 bit stereo (max 45kHz)');
    repeat c:=readkey; until (c in ['0','1','2','3','4']);
    case c of
      '0': exit;
      '1': SBNo:=1;
      '2': SBNo:=3;
      '3': SBNo:=4; { also 2,5 }
      '4': SBNo:=6;
    end;
    writeln(#13#10' Soundblaster baseport 2X0h ?');
    write(' X = ');
    repeat c:=readkey; until (c in ['0'..'9']);
    writeln(c);
    sdev_hw_base := $200 + $10*( ord(c) - ord('0') );
    write(#13#10' 8 bit DMA channel (0,1,3) ? ');
    repeat c:=readkey; until (c in ['0','1','3']);
    writeln(c);
    sdev_hw_dma8 := ord(c)-ord('0');
    if SBNo=6 then
      begin
        write(#13#10' 16 bit DMA (5,6,7) ? ');
        repeat c:=readkey; until (c in ['5','6','7']);
        writeln(c);
        sdev_hw_dma16 := ord(c)-ord('0');
      end;
    write(#13#10' IRQ (2,5,7) ? ');
    repeat c:=readkey; until (c in ['2','5','7']);
    writeln(c);
    sdev_hw_irq := ord(c) - ord('0');
    forceto( SBNo, sdev_hw_dma8, sdev_hw_dma16, sdev_hw_irq, sdev_hw_base );
    InputSoundblasterValues:=true;
  end;

procedure sbInit;
begin
    sdev_configured := false;
    set_hwflags( false, false, false, false );
    set_hwconfig( $220, 7, 1, 5 );
    set_hw_dsp( 0 );
    sdev_hw_dspv := 0;
    set_mode( 0, false, false, false );
    ISRUserCallback := nil;
end;

procedure sbDone;
begin
end;

begin
  sbInit;
end.
