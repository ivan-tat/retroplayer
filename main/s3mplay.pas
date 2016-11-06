{$M 16000,0,0}
{$I-,X+,V-,G+,D+}
unit s3mplay;

INTERFACE

uses
    types,
    s3mtypes,
    s3mvars,
    voltab,
    posttab,
    fillvars;

CONST
    PLAYER_VERSION: PChar = '1.70.1';

CONST
      { error constants }
      noerror              = 0;
      notenoughmem         = -1;
      wrongformat          = -2;
      filecorrupt          = -3;
      filenotexist         = -4;
      packedsamples        = -5; { sorry I don't know about DigiPlay 3.0 ADPCM packing
                                   was anyway not used yet in S3Ms ... }
      Allreadyallocbuffers = -6; { don't try to allocate memory for buffers twice }
      nota386orhigher      = -7; { for playing any sound we need a 386 or higher -
                                   sorry but I optimized it for a 486 (pipeline etc.) and it
                                   runs fine on a 386 ;)
                                   (Hey guys a 486DX is not that expensive - for
                                   the same price I got an slow 386SX in 1991) }
      nosounddevice        = -8; { before 'start playing' - set a sounddevice ! }
      noS3Minmemory        = -9; { before 'start playing' - load a S3M ! }
      ordercorrupt         = -10; { if there's no playable entry in order -> that would cause an endless
                                    loop in readnotes if you try to play it }
      internal_failure     = -11; { I'm sorry if this happend :( }
      sample2large         = -12; { I can't handle samples >64511 }

{ variables for public }
VAR load_Error:integer;
    player_Error:integer;

FUNCTION  load_s3m(name:string):BOOLEAN;        { load S3M module into memory }
PROCEDURE done_module;                          { free memory used by S3M }
FUNCTION  Init_device(input:byte):boolean;       { = false if set device failed }
FUNCTION  Init_S3Mplayer:boolean;                { init DMAbuffer,tickbuffer,volumetable and some variables }
PROCEDURE Done_S3Mplayer;                       { free buffers used by player }
PROCEDURE setSampleRate(var SR:word;stereo:boolean); { set SampleRate for playing mono/stereo - higher frequency
                                                           means more processor time for calc sound
                                                           stereo question is because possible stereo/mono rates may differ }
FUNCTION  startplaying(var A_stereo,A_16Bit:boolean;LQ:Boolean):Boolean;
                                                              (* play totaly in background - you have nothin else to do
                                                                 for continue playing !
                                                                 It'll interrupt your program itself and calculate
                                                                 the next data is required *)
procedure set_mastervolume(vol:byte);
procedure set_ST3order(new:boolean);             (* look at ST3order *)
{ To get some infos : }
function getspeed:byte;
function gettempo:byte;
function get_mvolume:byte;
function get_delay:byte;
function getSamplerate:word;
function getusedEMSsmp:longint;    { get size of samples in EMS }
function getusedEMSpat:longint;    { get size of patterns in EMS }

{ not supported functions: }
FUNCTION getuseddevice(var typ:byte;var base:word;var dma8,dma16:byte;var irq:byte):byte;
FUNCTION load_specialdata(var p):boolean; { allocate memory and load special data from file }

(* Patterns *)
procedure setPattern( index: integer; p_seg: word );
procedure setPatternInEM( index: integer; logpage, part: byte );
function getPattern( index: integer ): pointer;
function isPatternInEM( index: integer ): boolean;
function getPatternLogPageInEM( index: integer ): byte;
function getPatternPartInEM( index: integer ): byte;

IMPLEMENTATION

uses
    cpu,
    memset,
    crt,
    dos,
    dosproc,
    pic,
    EMStool,
    sbctl,
    blaster,
    effvars,
    mixvars,
    mixer_,
    mixer,
    filldma,
    mixing,
    readnote;

CONST DMAbuffersize=8*1024; { <- maximum size of DMAbuffer }

{ Internal variables : }
VAR
    PROC386:boolean;      { A 386 processor ? }
(* UNUSED: filename: string; (* name of file currently in memory *)
    buffersreserved: boolean;
    sounddevice: boolean;
    Samplerate: word;
    { S3M flags : }
    { own Flags : }
    { some saved values for correct restoring former status : }
    oldexitproc  :pointer;

{ getuseddevice is not implemented yet }
FUNCTION getuseddevice(var typ:byte;var base:word;var dma8,dma16:byte; var irq:byte):byte;
{ = 0 ... no device set / = 1 ... use SB mixing / > 1 ... other devices not supported yet }
{ typ ... up2now only SB typ - look at BLASTER.PAS }
begin end;

(* Patterns >> *)

procedure setPattern( index: integer; p_seg: word );
begin
    PATTERN[ index ] := p_seg;
end;

procedure setPatternInEM( index: integer; logpage, part: byte );
begin
    PATTERN[ index ] := $C000 + ( ( part and $3f ) shl 8 ) + logpage;
end;

function getPattern( index: integer ): pointer;
var
    p_seg: word;
begin
    p_seg := PATTERN[ index ];
    if ( p_seg >= $C000 ) then
        getPattern := ptr( FrameSEG[0], ( ( p_seg shr 8 ) and $3f ) * patlength )
    else
        getPattern := ptr( p_seg, 0 );
end;

function isPatternInEM( index: integer ): boolean;
begin
    isPatternInEM := PATTERN[ index ] >= $C000;
end;

function getPatternLogPageInEM( index: integer ): byte;
begin
    getPatternLogPageInEM := PATTERN[ index ] and $ff;
end;

function getPatternPartInEM( index: integer ): byte;
begin
    getPatternPartInEM := ( PATTERN[ index ] shr 8 ) and $3f;
end;

(* << Patterns *)

PROCEDURE done_module;
var i:word;
    p:pointer;
    psmp:PsmpHeader;
  BEGIN
    if not mod_isLoaded then exit;
    { Free samples & instruments : }
    for i:=1 to MAX_INSTRUMENTS do
      begin
        psmp:=addr(Instruments^[i]);
        if (psmp^.typ=1) then
          begin
            if psmp^.mempos<$f000 then { no EMS instrument }
              begin
                p:=ptr(psmp^.mempos,0);
                psmp^.mempos:=0;
                if p<>Nil then freedosmem(p);
              end;
          end;
        Instruments^[i,0]:=0;
      end;
    { Free patterns : }
    for i:=0 to MAX_patterns do
      begin
        if ( not isPatternInEM( i ) ) then
          begin
            p := getPattern( i );
            if p<>Nil then freedosmem(p);
            setPattern( i, 0 );
          end;
      end;
    if EMSpat then { patterns in EMS }
      begin
        EMSfree(savHandle);
        EMSfree(patEMShandle);
        EMSpat:=false;
      end;
    if EMSsmp then { samples in EMS }
      begin
        EMSfree(smpEMShandle);
        EMSsmp:=false;
      end;
    mod_isLoaded:=false;
  END;

PROCEDURE Done_S3Mplayer;
  begin
    restore_irq;
    freeVolumeTable;
    if AllocBuffer<>Nil then freeDOSmem(AllocBuffer);
    if Tickbuffer<>Nil then freeDOSmem(TickBuffer);
    buffersreserved:=false;
    playbuffer:=Nil;
    DMABuffer:=Nil;
  end;

PROCEDURE NewExitRoutine; Far;
  begin
    stop_play; { halt SB :) }
    done_module;
    if buffersreserved then done_S3Mplayer else restore_irq;
    exitproc:=oldexitproc;
  end;

{$I LOADPROC.INC}

FUNCTION Init_device(input:byte):boolean;
{  input= 0 ... use settings in BLASTER unit
        = 1 ... hardware autodetect SB
        = 2 ... read blaster enviroment
        = 3 ... input by hand }
  begin
    Init_device:=false;
    if Input = 0 then { 'checkthem' not yet implemented } sounddevice:=true
    else
    if Input = 1 then Sounddevice:=DetectSoundblaster(true)
    else
    if Input = 2 then Sounddevice:=UseBlasterEnv
    else
    if Input = 3 then Sounddevice:=InputSoundblasterValues;
    Init_device:=Sounddevice;
  end;

function checkoverride(var p;l:word):boolean; assembler;
  asm
    mov     bx,1
    mov     ax,word ptr [p+2]
    rol     ax,4
    and     al,00fh
    add     ax,l
    jc      @@anoverride
    xor     bx,bx
@@anoverride:
    mov     ax,bx
  end;

FUNCTION Init_S3Mplayer:boolean;
var p:pArray;
  begin
    Init_S3Mplayer:=false;
    if not proc386 then begin player_error:=nota386orhigher;exit end;
    if buffersreserved then begin player_error:=Allreadyallocbuffers;Init_S3Mplayer:=true;exit end;
    { buffersreserved = false ! }
    if ( not allocVolumeTable ) then begin player_error:=notenoughmem;exit end;
    if not getdosmem(Allocbuffer,(DMABuffersize+15)*2) then begin player_error:=notenoughmem;exit end;
    { ok and now check for DMA page overrides }
    if checkoverride(Allocbuffer^,DMAbuffersize) then
      { it's a page override in first DMAbuffer - use second }
      begin
        { Can't free the first part - sorry it's not possible with a DOS function }
        { I know I can creat my own PSP etc., maybe later, ok ? - it's a problem  }
        { for final activities. }
        p:=allocBuffer;
        DMAbuffer:=ptr(seg(p^)+Dmabuffersize div 16,0);
        {$IFDEF BETATEST}
        write(' Use second part of DMAbuffer ... at ',seg(Dmabuffer^));
        {$ENDIF}
      end
    else
      begin
        { use first buffer and free the rest }
        {setsize(Allocbuffer,DMABuffersize);}
        DMAbuffer:=AllocBuffer;
        {$IFDEF BETATEST}
        write(' Use first part of DMAbuffer ... at ',seg(DMAbuffer^));
        {$ENDIF}
      end;
    {
      in tick buffer we calc one DMA buffer half - that are dmabuffersize/2 words
    }
    if not getdosmem(Tickbuffer,DMAbuffersize) then
      begin
        freedosmem(Allocbuffer);
        freeVolumeTable;
        player_error:=notenoughmem;
        exit
      end;
    playBuffer:=DMABuffer;
    buffersreserved:=true;
    { clear those buffers : }
    fillchar(dmabuffer^,dmabuffersize,0);
    fillchar(tickbuffer^,dmabuffersize,0);
    Init_S3Mplayer:=true;
  end;

PROCEDURE setSampleRate(var SR:word;stereo:boolean);
var w:word;
    i,j:byte;
  begin
    (*sbAdjustMode(SR,stereo);*)
    Samplerate :=SR;

    if LQmode then
      Userate:=SR div 2
    else
      Userate:=SR;

    w:=(1+ord(stereo))*(trunc(1000000/(trunc(1000000/Userate))/FPS)+1);
    i:=DMAbuffersize div w;
    j:=1;while j<i do j:=j shl 1;j:=j shr 1;
    if LQmode then j:=j shr 1;
    for i:=0 to j-1 do
      dmarealbufsize[i]:=i*w;
    NumBuffers:=j;
  end;

function getspeed:byte;
  begin
    getspeed:=curspeed;
  end;

function gettempo:byte;
  begin
    gettempo:=curtempo
  end;

var inside:boolean;

procedure PlaySoundCallback; far;
begin
    while ( inside ) do
    begin
        (* wait *)
    end;

    inside := true;
    DMAhalf := ( DMAhalf+1 ) and ( numbuffers-1 );
    inside := false;

    if ( rastertime ) then
    begin
        (* TODO *)
    end;

    fill_dmabuffer;

    if ( rastertime ) then
    begin
        (* TODO *)
    end;
end;

procedure Initchannels;
var i:byte;
  begin
    for i:=0 to usedchannels-1 do
      begin
        channel[i].VibTabOfs:=ofs(sinuswave);
        channel[i].TrmTabOfs:=ofs(sinuswave);
      end;
  end;

procedure set_mastervolume(vol:byte);
  begin
    if vol>127 then vol:=127;
    mvolume:=vol;
    calcposttable(mvolume,_16bit);
  end;

function get_mvolume:byte;
  begin
    get_mvolume:=mvolume;
  end;

function get_delay:byte;
  begin
    get_delay:=patterndelay;
  end;

function getSamplerate:word;
  begin
    getSamplerate:=Samplerate;
  end;

function getusedEMSsmp:longint;    { get size of samples in EMS }
begin
    if EMSsmp then
        getusedEMSsmp:=16*EmsGetHandleSize(smpEMShandle)
    else
        getusedEMSsmp:=0;
end;

function getusedEMSpat:longint;    { get size of patterns in EMS }
begin
    if EMSpat then
        getusedEMSpat:=16*EmsGetHandleSize(patEMShandle)
    else
        getusedEMSpat:=0;
end;

procedure set_ST3order(new:boolean);
var i:byte;
  begin
    playOption_ST3Order:=new;
    if new then
      begin
        { search for first '--' }
        i:=0;
        while (i<ordnum-1) and (order[i]<255) do inc(i);
        dec(i);
        lastorder:=i
      end
    else
      begin
        { just for fun (is not important,
          you can also do simply lastorder=ordnum-1 }
        i:=ordnum-1;
        while (i>0) and (order[i]>=254) do dec(i);
        lastorder:=i;
      end;
  end;

FUNCTION startplaying(var A_stereo,A_16Bit:boolean;LQ:Boolean):boolean;
var key:boolean;
    p:parray;
    count: word;
  begin
    startplaying:=false;
    player_error:=0;
    lqmode:=LQ;
    A_stereo := A_Stereo and sdev_caps_stereo;
    A_16Bit := A_16Bit and sdev_caps_16bit;
    if not sounddevice then begin player_error:=nosounddevice;exit; end; { sorry no device was set }
    if not mod_isLoaded then begin player_error:=noS3Minmemory;exit end; { hmm load it first ;) }
    set_ready_irq( @PlaySoundCallback );
    Initblaster(Samplerate,a_stereo,a_16Bit);
    setSamplerate(Samplerate,a_stereo);

    (* now after loading we know if signed data or not *)
    calcVolumeTable( modOption_SignedData );

    calcposttable(mvolume,A_16bit);
    curtick:=1; { last tick -> goto next note ! }
    curLine:=0; { <- next line to read from }
    {$IFDEF BETATEST}
    curorder:=startorder;
    {$ELSE}
    curOrder:=0; { <- next order to read from }
    {$ENDIF}
    curpattern:=order[0]; { next pattern to read from }
    patterndelay:=0;      { no patterndelay at start of course ! }
    Ploop_on:=false;
    Ploop_to:=0;
    curspeed:=initspeed;set_tempo(inittempo);
    set_ST3order(playOption_ST3Order); { <- don't remove this ! it's important ! (setup lastorder) }
    EndOfSong:=false;TooSlow:=false;
    TickBytesLeft:=0;       { emmidiately next tick }
    Initchannels;

    count := DMArealbufsize[1];
    if ( lqmode ) then count := count * 2;

    (* loop through whole DMAbuffer *)
    sbSetupDMATransfer( DMABuffer, count * numBuffers, true );

    DMAhalf := numbuffers-1;
    lastready := numbuffers;

    (* calc all buffer parts *)
    fill_dmabuffer;

    (* double buffering *)
    play_firstblock( count );

    (* ok, now everything works in background *)
    startplaying:=true;
  end;

VAR i:byte;

procedure calcwaves;
  begin
    for i:=0 to 63 do
      begin
        squarewave[i]:=255*ord(i<64);
        sinuswave[i] :=round(sin(pi/32*i)*(127));
        rampwave[i]  :=i*2-127;
      end;
  end;

BEGIN
  inside:=false;
  PROC386:=isCPU_i386;
  calcwaves;
  buffersreserved:=false;
  sounddevice:=false;
  oldexitproc:=exitproc;
  exitproc:=@newExitRoutine;
  initVolumeTable;
  DMAbuffer:=Nil;
  AllocBuffer:=Nil;
  playBuffer:=Nil;
  Tickbuffer:=Nil;
  Samplerate:=22000; { not the highest but nice sounding samplerate :) }
  Userate:=22000;
  playOption_LoopSong:=false;
  playOption_ST3Order:=false;   { Ok let's hear all patterns are saved ... }
  rastertime:=false;
  useEMS:=EMSinstalled;      { more space for Modules ! }
  if not getdosmem(instruments,MAX_INSTRUMENTS*sizeof(TInstr)) then
    begin
      asm
        mov     ax,3
        int     10h
      end;
      writeln(' Hey S3M-Player needs some DOSmem (programmers info: lower PAS-heap !) ');
      halt(1);
    end;

  FOR i:=1 TO MAX_INSTRUMENTS DO
    BEGIN
      Instruments^[i,0]:=0;
    END;
  for i := 0 to MAX_patterns-1 do setPattern( i, 0 );
  FPS:=70;
  LQmode:=false;
END.
