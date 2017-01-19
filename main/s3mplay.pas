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
FUNCTION  Init_S3Mplayer:boolean;                { init DMABuf,mixBuf,volumetable and some variables }
PROCEDURE Done_S3Mplayer;                       { free buffers used by player }
PROCEDURE playSetMode(m16bits, mStereo: boolean; mRate: word);
FUNCTION  startplaying(var A_stereo,A_16Bit:boolean;LQ:Boolean):Boolean;
    (* play totaly in background - you have nothin else to do for continue playing !
        It'll interrupt your program itself and calculate the next data is required *)
procedure set_mastervolume(vol:byte);
procedure set_ST3order(new:boolean);             (* look at ST3order *)
{ To get some infos : }
function getspeed:byte;
function gettempo:byte;
function get_mvolume:byte;
function get_delay:byte;
function getSamplerate:word;
function getusedEMSsmp:longint;    { get size of samples in EMS }

{ not supported functions: }
FUNCTION getuseddevice(var typ:byte;var base:word;var dma8,dma16:byte;var irq:byte):byte;
FUNCTION load_specialdata(var p):boolean; { allocate memory and load special data from file }

IMPLEMENTATION

uses
    cpu,
    memset,
    printf,
    strutils,
    crt,
    dos,
    dosproc,
    emstool,
    pic,
    sndctl_t,
    sbctl,
    blaster,
    effvars,
    mixvars,
    mixer_,
    mixer,
    filldma,
    mixing,
    effects,
    readnote;

(*$l s3mplay.obj*)

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
    if EMSsmp then { samples in EMS }
      begin
        EMSfree(smpEMShandle);
        EMSsmp:=false;
      end;
    patListDone;
    mod_isLoaded:=false;
  END;

PROCEDURE Done_S3Mplayer;
  begin
    restore_irq;
    freeVolumeTable;
    sndDMABufDone(@sndDMABuf);
    if mixBuf<>Nil then freeDOSmem(mixBuf);
    buffersreserved:=false;
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

FUNCTION Init_S3Mplayer:boolean;
var p:pArray;
  begin
    Init_S3Mplayer:=false;
    if not proc386 then begin player_error:=nota386orhigher;exit end;
    if buffersreserved then begin player_error:=Allreadyallocbuffers;Init_S3Mplayer:=true;exit end;
    { buffersreserved = false ! }
    if ( not allocVolumeTable ) then begin player_error:=notenoughmem;exit end;

    if (not sndDMABufAlloc(@sndDMABuf, DMA_BUF_SIZE_MAX)) then
    begin
        player_error := notenoughmem;
        exit;
    end;
    {
      in tick buffer we calc one DMA buffer half - that are DMA_BUF_SIZE_MAX/2 words
    }
    if (not getdosmem(mixBuf, sndDMABuf.buf^.Size)) then
      begin
        sndDMABufFree(@sndDMABuf);
        freeVolumeTable;
        player_error:=notenoughmem;
        exit
      end;
    buffersreserved:=true;
    { clear those buffers : }
    fillchar(mixBuf^,DMA_BUF_SIZE_MAX,0);
    Init_S3Mplayer:=true;
  end;

PROCEDURE playSetMode(m16bits, mStereo: boolean; mRate: word);
var
    outbuf: PSNDDMABUF;
    b: byte;
    s: boolean;
    c: byte;
    r: word;
    tmp: word;
    i: byte;
    fmt: THWSMPFMT;
begin
    (*sbAdjustMode(mRate,mStereo,m16bits);*)

    case m16bits of
        false:
            begin
                b := 8;
                s := false;
            end;
        true:
            begin
                b := 16;
                s := true;
            end;
    end;

    case mStereo of
        false: c := 1;
        true: c := 2;
    end;

    SampleRate := mRate;

    outbuf := @sndDMABuf;

    r := mRate;
    if (playOption_LowQuality) then r := r shr 1;

    set_sample_format(@(outbuf^.format), b, s, c);
    (*tmp := longint(1000000) div r;
    setMixMode(c, r, ((longint(1000000) div tmp) div playOption_FPS)+1);*)
    setMixMode(c, r, trunc(trunc(1000000/trunc(1000000/r))/playOption_FPS)+1);

    outbuf^.frameSize := mixBufSamplesPerChannel;
    if (b = 16) then outbuf^.frameSize := outbuf^.frameSize shl 1;
    if (c = 2) then outbuf^.frameSize := outbuf^.frameSize shl 1;

    i := outbuf^.buf^.Size div outbuf^.frameSize;
    outbuf^.framesCount := 1;
    while outbuf^.framesCount < i do outbuf^.framesCount := outbuf^.framesCount shl 1;
    outbuf^.framesCount := outbuf^.framesCount shr 1;
    if (playOption_LowQuality) then outbuf^.framesCount := outbuf^.framesCount shr 1;
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
    sndDMABuf.frameActive := (sndDMABuf.frameActive + 1) and (sndDMABuf.framesCount - 1);
    inside := false;

    fill_dmabuffer(mixbuf, @sndDMABuf);
end;

procedure Initchannels;
var i:byte;
  begin
    for i:=0 to usedchannels-1 do
      begin
        channel[i].wVibTab:=ofs(sinuswave);
        channel[i].wTrmTab:=ofs(sinuswave);
      end;
  end;

procedure set_mastervolume(vol:byte);
  begin
    if vol>127 then vol:=127;
    mvolume:=vol;
    calcposttable(mvolume, sdev_mode_16bit);
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
    playOption_LowQuality := LQ;
    A_stereo := A_Stereo and sdev_caps_stereo;
    A_16Bit := A_16Bit and sdev_caps_16bit;
    if not sounddevice then begin player_error:=nosounddevice;exit; end; { sorry no device was set }
    if not mod_isLoaded then begin player_error:=noS3Minmemory;exit end; { hmm load it first ;) }
    set_ready_irq( @PlaySoundCallback );
    Initblaster(a_16Bit,a_stereo,Samplerate);
    playSetMode(a_16bit,a_stereo,Samplerate);

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
    set_speed(initspeed);
    set_tempo(inittempo);
    set_ST3order(playOption_ST3Order); { <- don't remove this ! it's important ! (setup lastorder) }
    EndOfSong:=false;
    sndDMABuf.flags_Slow := false;
    mixTickSamplesPerChannelLeft:=0;    (* emmidiately next tick *)
    Initchannels;

    count := sndDMABuf.frameSize;
    if (playOption_LowQuality) then count := count * 2;

    (* loop through whole DMA buffer *)
    sbSetupDMATransfer(sndDMABuf.buf^.Data, count * sndDMABuf.framesCount, true);

    sndDMABuf.frameActive := sndDMABuf.framesCount - 1;
    sndDMABuf.frameLast := sndDMABuf.framesCount;

    (* calc all buffer parts *)
    fill_dmabuffer(mixbuf, @sndDMABuf);

    (* double buffering *)
    play_firstblock( count );

    (* ok, now everything works in background *)
    startplaying:=true;
  end;

procedure calcwaves;
VAR i:byte;
  begin
    for i:=0 to 63 do
      begin
        squarewave[i]:=255*ord(i<64);
        sinuswave[i] :=round(sin(pi/32*i)*(127));
        rampwave[i]  :=i*2-127;
      end;
  end;

procedure s3mplayInit;
var
    i: integer;
begin
  inside:=false;
  PROC386:=isCPU_i386;
  calcwaves;
  buffersreserved:=false;
  sounddevice:=false;
  initVolumeTable;
  sndDMABufInit(@sndDMABuf);
  mixBuf:=Nil;  (* FIXME: initMixBuf() *)
  Samplerate:=22000; { not the highest but nice sounding samplerate :) }
  mixSampleRate := Samplerate;
  (* TODO: +mixChannels, +mixBufSamplesPerChannel *)
  playOption_LoopSong:=false;
  playOption_ST3Order:=false;   { Ok let's hear all patterns are saved ... }
  playOption_FPS := 70;
  playOption_LowQuality := false;
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
    patListInit;
end;

procedure s3mplayDone;
begin
    stop_play;
    done_module;
    if buffersreserved then done_S3Mplayer else restore_irq;
end;

procedure register_s3mplay; far; external;

begin
    register_s3mplay;
end.
