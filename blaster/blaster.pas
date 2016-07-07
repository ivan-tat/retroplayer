{$A+,B-,D+,E-,F-,G+,I-,L+,N-,O-,P-,Q-,R-,S-,T-,V-,X+,Y+}
{$M 16384,0,655360}
Unit Blaster;

{ -------------------------------------------------------------------- }
{ This is a specially for detecting and using the soundblaster card.   }
{ autodetect routines work for dsp adresses 210 to 280,                }
{ 8bit dmachannels 0,1,3/ 16bit dmachannel 5 and for interrupts 2,5,7  }
{ (known problems with IRQ2)                                           }
{ -------------------------------------------------------------------- }
{ no more work on autodetection - wait for craigs source, takes better }
{ advantage to our knowledge about different SB models, we got in last }
{ time. i.e. you can read IRQ#/DMA8/DMA16 from mixer port on SB16      }
{ --------------------------------------------------------------------- }
{ I read about 16Bit improvment for SB PRO3/4.0 but don't know about -  }
{ anybody can help me with that ?                                       }
{ how to use it ... any specifications for it ?                         }
{ --------------------------------------------------------------------- }
{ STATUS: TESTED ON SB16/ASP,SB PRO2,SB2.0                              }
{         COMMENTs READY                                                }
{   KNOWN PROBLEMS : - IRQ2 detection is wrong on some maschines        }
{                    - no 16bit IRQ autodetection :(                    }
{                    - no check if values are right in                  }
{                               * UseBlasterEnv                         }
{                               * InputBlasterValues                    }
{                    - no mixedup detection (i.e. some values as        }
{                      parameters and detect the rest)                  }
{                      I though about this while I had a SB PRO and a   }
{                      SB16 at the same time 'inside' on base 220h,240h }
{ --------------------------------------------------------------------- }

Interface

CONST defaultvolume=31;    { full power =;) }
      IRQ_TABLE:array[0..15] of byte = ($08,$09,$0a,$0B,$0C,$0D,$0E,$0F,
                                        $70,$71,$72,$73,$74,$75,$76,$77);

PROCEDURE Forceto(typ,dma,dma16,irq:byte;dsp:word);     (* force to use these values for playing *)
FUNCTION UseBlasterEnv:boolean;                         (* use values set in enviroment BLASTER *)
FUNCTION InputSoundblasterValues:Boolean;               (* Input Soundbaster values by hand (in textmode) *)
FUNCTION DetectSoundblaster(prot:boolean):Boolean;      (* detects Soundblastercard *)

FUNCTION Detect_DSP_Addr(prot:boolean):Boolean;         (* detects DSP ADDR *)
FUNCTION Detect_DMA_Channel_IRQ(prot:boolean):Boolean;  (* detects DMA Channel,DSPIRQ *)
FUNCTION Get_BlasterVersion:Word;                       (* reads the detected soundblaster version *)

procedure check_samplerate(var rate:word;var stereo:boolean); (* check min/max samplerate *)

procedure play_firstBlock(length:word);
  (* set the SBinterrupt to "interrupt" every "length" bytes
     the best way to use it, is to play the half buffersize and then
     write the new data in the allready played part
     - you have to setup DMA controller by calling setupDMATransfer *)
PROCEDURE play_oneBlock(p:pointer;length:word);
  (* it's a play routine for playing only one buffer (NOT CONTINOUSLY
     PLAYING ! - it does not work in that way on a SB16 !)
     we use non autoinit mode here for all SBs...
     this proc. setup DMA controller *)
PROCEDURE Initblaster(var frequ:Word;stereoon,_16Biton:Boolean);
  (* set frequency and stereo/mono and 8/16 Bit mode *)

PROCEDURE set_ready_irq(p:pointer);
  (* set user irq - (irq if buffer ends !)
     attention look at my default irq !
     You need those two port commands you'll find there !
     And pay attention to 16bit/8bit mode on SB16 (different
     ackknowledgement ports) *)
PROCEDURE stop_play;         (* stops playing ! *)
PROCEDURE pause_play;        (* stops playing, but you can continue
                                with "continue_play" *)
PROCEDURE continue_play;     (* continues playing after pause play *)
PROCEDURE restore_irq;       (* restore old interrupt vector *)
PROCEDURE set_sign(signed:boolean); (* sets flag to play signed data
               - does only work on SB16 (set it before start playing) *)
PROCEDURE setvolume(vol:Byte);   (* what do you think ? *)
function  sbGetDMACounter:word;      (* It's for 8 & 16 Bit mode to get the DMA counter *)
procedure writelnSBConfig;       (* what do you expect ? - write current setup to screen,
                                    but detect SB before calling that proc. *)

procedure sbInit;
procedure sbDone;

Implementation

uses dos,dma,sbio,sbctl,pic,crt;

{ Flags and variables for detect part : }
VAR SB_Detect:Boolean;              { Flag if SB is detected }
    DSPIRQ_Detect:Boolean;          { Flag if IRQ number is detected }
    DSPADR_Detect:Boolean;          { Flag if Baseaddress is detected }
    DMACHN_Detect:Boolean;          { Flag if DMAchannel is detected }
    MIXER_Detect:Boolean;           { Flag if Mixerchip is detected }
    SBVersHi:Byte;                  { Soundblaster version major }
    SBVersLo:Byte;                  { Soundblaster version minor }

    check:byte;                     { for detecting }
    savvect:pointer;                {  "       "    }

{ Sorry no cool macro like in C is possible }
function loword(l:longint):word; assembler;
  asm
    mov         ax,word ptr(l)
  end;

function hiword(l:longint):word; assembler;
  asm
    mov         ax,word ptr(l+2)
  end;

function sbGetDMACounter:word;
{ get the dma base counter of dmachannel is used by SB
  you can check if sound transfer does work ;) }
var
    ch: byte;
begin
    if ( _16Bit ) then
        ch := sdev_hw_dma16
    else
        ch := sdev_hw_dma8;
    sbGetDMACounter := dmaGetCounter( ch );
end;

(* call this if you want to do continues play *)
procedure play_firstBlock(length:word);
begin
    sbSetupDSPTransfer( length, _16bit, true );
end;

PROCEDURE play_oneBlock(p:pointer;length:word);
{ call this if you want to play only ONE (!) block - I'm sure you can do
  continues play with this proc. on SBs <SB16 (I have seen that often in
  other sources, but it'll definitly not work on a SB16 ! It'll cause
  'ticks' }
begin
    setupDMATransfer( p, length, false );
    sbSetupDSPTransfer( length, _16bit, false );
end;

{ -------------------- continue commenting here ---------------------- }

procedure setDSPTimeConst( tc: byte );
(* Setup samplerate with time constant, take this:
   TC = 256 - TRUNC( 1 000 000 / SAMPLERATE ) *)
begin
    sbioDSPWrite( dsp_addr, $40 );
    sbioDSPWrite( dsp_addr, tc );
end;

procedure setDSPFrequency( freq: word );
begin
    sbioDSPWrite( dsp_addr, $41 );
    sbioDSPWrite( dsp_addr, hi( freq ) );
    sbioDSPWrite( dsp_addr, lo( freq ) );
end;

procedure setDSPSampleRate( freq: word; stereo: boolean );
var
    tc: byte;
begin
    (* calculate timeconstant - pay attention on SB PRO you have to setup
       2*samplerate in stereo mode (so call it byterate) - on SB16 not ! *)
    if ( sbno = 6 ) or not stereo then
    begin
        tc := 256 - 1000000 div freq;
        freq := 1000000 div ( 256 - tc );
    end else begin
        tc := 256 - 1000000 div ( 2 * freq );
        freq := ( 1000000 div ( 256 - tc ) ) div 2;
    end;
    if ( sbno < 6 ) then
        setDSPTimeConst( tc )
    else
        setDSPFrequency( freq );
end;

PROCEDURE Initblaster(var frequ:Word;stereoon,_16Biton:boolean);
{ Initblaster does this :   1. check samplerates for its borders
                            2. Reset DSP chip
                            3. setup samplerate
                            4. setup stereo/mono mode
 if you want to play signed data on SB16, call 'set_sign' after Initblaster }

var tc:byte;
  begin
    { first reset SB : }
    asm
      mov    dx,dsp_addr
      add    dx,0eh
      in     al,dx
      inc    dx
      in     al,dx
    end;
    stop_play;
    { Now init : }
    check_samplerate(frequ,stereoon);
    _16bit:=(SBNo=6) and _16Biton;
    stereo:=stereoon;
    sbioDSPReset( dsp_addr );
    setDSPSampleRate( frequ, stereo );
    { setup stereo option on SB PRO - on SB16 it's set in DSP command }
    if stereo and (SBNo<>6) then
      sbMixerWrite($0e,sbMixerRead($0e) or $02); { stereo option on (only SB PRO) }
    if SBNo in [2,4,5] then
      sbMixerWrite($0e,sbMixerRead($0e) or $20); { filter option off (only SB PRO) }
    speaker_on;
  end;

{ -------------- now the procedures for my old autodetection ------------- }
{ No comments about it - it's old ;)                                       }
procedure irq2; interrupt;
var a:byte;
begin
    check:=2;
    picEOI( 2 );
    a:=port[dsp_addr+$0e]
end;
procedure irq5; interrupt;
var a:byte;
begin
    check:=5;
    picEOI( 5 );
    a:=port[dsp_addr+$0e]
end;
procedure irq7; interrupt;
var a:byte;
begin
    check:=7;
    picEOI( 7 );
    a:=port[dsp_addr+$0e]
end;
procedure ready_irq; interrupt;
var a:byte;
begin
    check:=1;
    picEOI( 0 ); (* FIXME *)
    a:=port[dsp_addr+$0e]
end;

function hexword(w:word):string;
const hex:string= '0123456789ABCDEF';
  begin
   hexword:=hex[hi(w) div 16+1]+hex[hi(w) mod 16+1]+hex[lo(w) div 16+1]+hex[lo(w) mod 16+1];
  end;

FUNCTION Detect_DSP_Addr(prot:boolean):Boolean;
var p:word;
  begin
    if dspadr_detect then begin detect_dsp_addr:=true; exit end;
    if prot then writeln(' Now locating DSP-Adresse :'#13#10);
    detect_dsp_addr:=false;
    p:=$210;
    while not dspadr_detect and (p<$290) do
      begin
        if prot then write(' Trying ',hexword(p),' .... ');
        dspadr_detect := sbioDSPReset( p );
        if not dspadr_detect then
          begin
            inc(p,$10);
            if prot then write('not ');
          end;
        if prot then writeln('succesfull ');
      end;
    if not dspadr_detect then exit;
    dsp_addr:=p;
    detect_dsp_addr:=true;
  end;

FUNCTION Detect_DMA_Channel_IRQ(prot:boolean):Boolean;
const irqs:array[1..3] of byte = (10,13,15); (* IRQ 2,5,7 *)
var oldv:array[1..5] of pointer;
    i,nr:byte;
    fr:word;
    ov1,ov2:byte;
  begin
  port[$0f] := $ff;
  asm sti end;
    if dmachn_detect then begin detect_DMA_Channel_irq:=true;exit end;
    if prot then writeln(#13#10' Now locating DMA-channel and IRQ :'#13#10);
    detect_dma_channel_irq:=false;
    if not dspadr_detect then exit;
    for i:=1 to 3 do
      begin
        getintvec(irqs[i],oldv[i]);
      end;
    setintvec(10,addr(irq2));
    setintvec(13,addr(irq5));
    setintvec(15,addr(irq7));
    Detect_DMA_Channel_irq:=false;
    picDisableIRQs( ($0004 + $0020 + $0080) and not $0004 );
    nr:=0;
    while (nr<4) and not DMACHN_Detect do
      begin
        if prot then write(' Trying Channel ',nr,' .... ');
        Check:=0;
        sdev_hw_dma8 := nr;
        fr:=10000;
        dmaMask( sdev_hw_dma8 ); (* was outp( 0x0a, dma_channel ) *)
        stop_play;speaker_off;
        Initblaster(fr,false,false);
        play_oneblock(ptr(0,0),1);
        delay(10);
        DMACHN_Detect:=check<>0;
        if not DMACHN_Detect then
          begin
            inc(nr);if nr=2 then nr:=3;
            if prot then write('not ');
          end;
        if prot then
          begin
            write('sucessful');
            if DMACHN_Detect then writeln(' with Interrupt ',IRQ_Table[check],' - IRQ ',check)
            else writeln;
          end;
      end;
    picEnableIRQs( ($0004 + $0020 + $0080) and not $0004 );
    for i:=1 to 3 do
      setintvec(irqs[i],oldv[i]);
    if not dmachn_detect then exit;
    Detect_DMA_Channel_irq:=true;
    DSPIRQ_detect:=true;
    sdev_hw_irq := Check;
    sbioDSPReset( dsp_addr );
  end;

function readDSPVersion: boolean;
var
    v_lo, v_hi: byte;
begin
    (* DSP 0xE1 - get DSP version *)
    if ( not sbioDSPWrite( dsp_addr, $e1 ) ) then
    begin
        readDSPVersion := false;
        exit;
    end;

    v_hi := sbioDSPRead( dsp_addr );
    if ( sbioError <> E_SBIO_SUCCESS ) then
    begin
        readDSPVersion := false;
        exit;
    end;

    v_lo := sbioDSPRead( dsp_addr );
    if ( sbioError <> E_SBIO_SUCCESS ) then
    begin
        readDSPVersion := false;
        exit;
    end;

    sbversLo := v_lo;
    sbversHi := v_hi;
    readDSPVersion := true;
end;

FUNCTION DetectSoundblaster(prot:boolean):Boolean;
  begin
    SBNo:=0;
    DetectSoundblaster:=false;
    SB_Detect:=False;
    DSPIRQ_Detect:=false;
    DSPADR_Detect:=false;
    DMACHN_Detect:=False;
    MIXER_Detect:=False;
    stereo_possible:=false;
    _16Bit_possible:=false;
    STEREO:=False;_16Bit:=False;
    if not Detect_DSP_Addr(prot) then
      begin
        if prot then writeln(' Can'#39't locate DSP-addresse ! ');
        exit;
      end;
    if ( not readDSPVersion ) then
    begin
        if ( prot ) then writeln('ERROR: Unable to get DSP chip version on this base address detected.');
        SBno := 0;
        exit;
    end;

    if (sbversHi<1) or (sbversHi>4) then
      begin
        if prot then writeln(' Sorry, unknown DSP chip version on this base address detected.');
        SBno:=0;
        exit;
      end;
    { for the first set SB1.0 - should work on all SBs }
    SBNo:=1;stereo_possible:=false;_16Bit_possible:=false;
    maxmonorate:=22050;maxstereorate:=0;
    stop_play;
    if not Detect_DMA_Channel_irq(prot) then
      begin
        if prot then writeln(' Can'#39't locate DMA-channel and IRQ ! ');
        sbNo:=0;
        exit;
      end;

    sbioDSPReset( dsp_addr );

{                              SBvers:
   SoundBlaster 1.0/1.5        1.xx
   SoundBlaster 2.0/2.5        2.xx
   SoundBlaster Pro/PRO3/PRO4  3.xx
   SoundBlaster 16/ASP         4.xx
}
    case sbversHi of
      1: begin
           SBNo:=1;stereo_possible:=false;_16Bit_possible:=false;
           maxmonorate:=22050;maxstereorate:=0
         end;
      2: begin
           SBNo:=3;stereo_possible:=false;_16Bit_possible:=false;
           maxmonorate:=44100;maxstereorate:=0
         end;
      3: begin
           SBNo:=2;stereo_possible:=true;_16Bit_possible:=false;
           maxmonorate:=44100;maxstereorate:=22700
         end;
      4: begin
           SBNo:=6;stereo_possible:=true;_16Bit_possible:=true;
           maxmonorate:=45454;maxstereorate:=45454
         end;
      else begin SBNo:=0;exit end;
    end;

    DetectSoundblaster:=true;
  end;

FUNCTION Get_BlasterVersion:Word;
  begin
    Get_BlasterVersion:=word(SBVersHi)*256+SBVersLo;
  end;

PROCEDURE set_ready_irq(p:pointer);
begin
    check:=0;
    getintvec( IRQ_Table[ sdev_hw_irq ], savvect );
    if p=Nil then p:=addr(ready_irq);
    setintvec( IRQ_Table[ sdev_hw_irq ], p );
    (* no changes for IRQ2 *)
    picDisableIRQs( ( 1 shl sdev_hw_irq ) and not ( 1 shl 2 ) );
end;

PROCEDURE restore_irq;
begin
    (* no changes for IRQ2 *)
    picEnableIRQs( ( 1 shl sdev_hw_irq ) and not ( 1 shl 2 ) );
    setintvec( IRQ_Table[ sdev_hw_irq ], savvect );
end;

FUNCTION ready:boolean;
  begin
    ready:=check>0;
  end;

PROCEDURE stop_play;
begin
    (* for 16bit modes : *)
    sbioDSPWrite( dsp_addr, $d0 );
    sbioDSPWrite( dsp_addr, $d9 );
    sbioDSPWrite( dsp_addr, $d0 );
    (* for 8bit modes : *)
    sbioDSPWrite( dsp_addr, $d0 );
    sbioDSPWrite( dsp_addr, $da );
    sbioDSPWrite( dsp_addr, $d0 );
    (* reset is the best way to make sure SB stops playing *)
    sbioDSPReset( dsp_addr );
    dmaMask( sdev_hw_dma8 ); (* was outp( 0x0a, dma_channel ) *)
end;

PROCEDURE pause_play;
begin
    if _16bit then
        sbioDSPWrite( dsp_addr, $d5 )
    else
        sbioDSPWrite( dsp_addr, $d0 );
end;

PROCEDURE continue_play;
begin
    if _16bit then
        sbioDSPWrite( dsp_addr, $d6 )
    else
        sbioDSPWrite( dsp_addr, $d4 );
end;

PROCEDURE set_sign(signed:boolean);
  begin
    signeddata:=signed;
  end;

procedure setfilter(how:boolean);
var b:byte;
  begin
    b:=sbMixerRead($0e);
    if how then { on } b:=b or $20 else b:=b and not $20;
    sbMixerWrite($0e,b); { switch the filter option }
  end;

procedure setvolume(vol:byte);
var b: byte;
begin
    if ( sbno < 6 ) then
    begin
        if vol >= 15 then vol := 15;
        vol := vol + ( vol shl 4 );
        sbMixerWrite( SBIO_MIXER_MASTER_VOLUME, vol );
        sbMixerWrite( SBIO_MIXER_DAC_LEVEL, vol );
    end else begin
        sbMixerWrite( SBIO_MIXER_MASTER_LEFT, vol );
        sbMixerWrite( SBIO_MIXER_MASTER_RIGHT, vol );
        sbMixerWrite( SBIO_MIXER_VOICE_LEFT, vol );
        sbMixerWrite( SBIO_MIXER_VOICE_RIGHT, vol );
    end;
end;

PROCEDURE Forceto(typ,dma,dma16,irq:byte;dsp:word);
  begin
    SB_Detect:=true;
    DSPIRQ_Detect:=true;
    DSPADR_Detect:=true;
    DMACHN_Detect:=true;
    stereo:=false;
    _16Bit:=false;

    MIXER_detect:=typ>1;
    stereo_possible:=typ in [2,4,5,6];
    _16Bit_possible:= typ=6;
    DSP_Addr:=dsp;
    sdev_hw_irq := irq;
    sdev_hw_dma8 := dma;
    sdev_hw_dma16 := dma16;
    SBNo:=typ;
    case typ of
      1: begin maxmonorate:=22050;maxstereorate:=0 end;
      2: begin maxmonorate:=44100;maxstereorate:=22050 end;
      3: begin maxmonorate:=44100;maxstereorate:=0 end;
      4: begin maxmonorate:=44100;maxstereorate:=22050 end;
      5: begin maxmonorate:=44100;maxstereorate:=22050 end;
      6: begin maxmonorate:=45454;maxstereorate:=45454 end;
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
    typ:=255;dma:=255;dma16:=255;irq:=255;dsp:=$ffff;
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
    write(#13#10' SB-Base : 2');write((dsp_addr div 16) mod 16);writeln('0h');
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
    dsp_addr:=$200+$10*(ord(c)-ord('0'));
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
    forceto( SBNo, sdev_hw_dma8, sdev_hw_dma16, sdev_hw_irq, dsp_addr );
    InputSoundblasterValues:=true;
  end;

procedure check_samplerate(var rate:word;var stereo:boolean);
  begin
    stereo:=stereo and stereo_possible;
    if rate<4000 then rate:=4000;
    if stereo then
       begin
         if rate>maxstereorate then rate:=maxstereorate;
       end
    else
      if rate>maxmonorate then rate:=maxmonorate;
  end;

procedure sbInit;
begin
  SB_Detect:=False;
  DSPIRQ_Detect:=false;
  DSPADR_Detect:=false;
  DMACHN_Detect:=False;
  MIXER_Detect:=False;
  stereo_possible:=false;
  _16Bit_possible:=false;
  STEREO:=False;_16Bit:=False;signeddata:=false;
  SBVersHi:=0;SBVersLo:=0;
  sdev_name := '';
  SBno:=0;
  DSP_Addr:=$220;
  sdev_hw_irq := 7;
  sdev_hw_dma8 := 1;
  sdev_hw_dma16 := 5;
end;

procedure sbDone;
begin
end;

begin
  sbInit;
end.
