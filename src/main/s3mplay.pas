{$M 16000,0,0}
{$I-,X+,V-,G+,D+}
unit s3mplay;

INTERFACE

uses
    types,
    s3mtypes,
    musins,
    muspat,
    s3mvars,
    voltab,
    posttab,
    fillvars;

(*$I defines.pas*)

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
VAR
    player_Error:integer;

function  player_load_s3m(name: String): Boolean;
    (* load S3M module into memory *)

procedure player_free_module;
    (* free memory used by module *)

function  player_init_device(input: Byte): Boolean;

function  player_init: Boolean;
    (* init DMABuf,mixBuf,volumetable and some variables *)

procedure player_free;
    (* free buffers used by player *)

procedure playSetMode(m16bits, mStereo: boolean; mRate: word);
function  playStart(var A_stereo, A_16Bit: Boolean; LQ: Boolean): Boolean;
procedure playSetMasterVolume(vol: Byte);
procedure playSetOrder(new: Boolean);   (* look at ST3order *)
function  playGetSpeed: byte;
function  playGetTempo: byte;
function  playGetMasterVolume: byte;
function  playGetPatternDelay: byte;
function  playGetSampleRate: word;

IMPLEMENTATION

uses
    cpu,
    dos_,
    stdio,
    string_,
    debug,
    strutils,
    crt,
    dos,
    ems,
    loads3m,
    pic,
    sndctl_t,
    sbctl,
    blaster,
    effvars,
    mixvars,
    mixchn,
    mixer_,
    mixer,
    filldma,
    mixing,
    effects,
    readnote;

const
    __FILE__ = 's3mplay.pas';

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

procedure player_free_module;
begin
    if not mod_isLoaded then exit;

    if (mod_Instruments <> nil) then
    begin
        musinsl_free(mod_Instruments);
        musinsl_delete(mod_Instruments);
    end;

    if (mod_Patterns <> nil) then
    begin
        muspatl_free(mod_Patterns);
        muspatl_delete(mod_Patterns);
    end;

    mod_isLoaded := false;
end;

PROCEDURE player_free;
  begin
    player_free_module;
    restore_irq;
    freeVolumeTable;
    sndDMABufDone(@sndDMABuf);
    if (mixBuf <> Nil) then
    begin
        _dos_freemem(seg(mixBuf^));
        mixBuf := nil;
    end;
    buffersreserved:=false;
  end;

function player_load_s3m(name: String): Boolean;
var
    p: PS3MLoader;
begin
    p := s3mloader_new;
    if (p = nil) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to initialize S3M loader.');
        load_error := notenoughmem;
        player_load_s3m := false;
        exit;
    end;
    s3mloader_clear(p);

    if (not s3mloader_load(p, name)) then
    begin
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to load S3M file.');
        s3mloader_free(p);
        s3mloader_delete(p);
        player_free_module;
        player_error := load_error;
        player_load_s3m := false;
        exit;
    end;

    s3mloader_free(p);
    s3mloader_delete(p);

    player_load_s3m := true;
end;

FUNCTION player_init_device(input:byte):boolean;
{  input= 0 ... use settings in BLASTER unit
        = 1 ... hardware autodetect SB
        = 2 ... read blaster enviroment
        = 3 ... input by hand }
  begin
    player_init_device:=false;
    if Input = 0 then { 'checkthem' not yet implemented } sounddevice:=true
    else
    if Input = 1 then Sounddevice:=DetectSoundblaster(true)
    else
    if Input = 2 then Sounddevice:=UseBlasterEnv
    else
    if Input = 3 then Sounddevice:=InputSoundblasterValues;
    player_init_device:=Sounddevice;
  end;

FUNCTION player_init:boolean;
var p:pArray;
    _seg: word;
  begin
    player_init:=false;
    if not proc386 then begin player_error:=nota386orhigher;exit end;
    if buffersreserved then begin player_error:=Allreadyallocbuffers;player_init:=true;exit end;
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
    if (_dos_allocmem((sndDMABuf.buf^.Size + 15) shr 4, _seg) <> 0) then
    begin
        sndDMABufFree(@sndDMABuf);
        freeVolumeTable;
        player_error:=notenoughmem;
        exit;
    end;
    mixBuf := ptr(_seg, 0);
    buffersreserved:=true;
    { clear those buffers : }
    fillchar(mixBuf^,DMA_BUF_SIZE_MAX,0);
    player_init:=true;
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

function playGetSpeed:byte;
  begin
    playGetSpeed:=playState_speed;
  end;

function playGetTempo:byte;
  begin
    playGetTempo:=playState_tempo
  end;

var inside:boolean;

procedure PlaySoundCallback; far;
var
    err: Boolean;
begin
    while ( inside ) do
    begin
        (* wait *)
    end;

    inside := true;
    sndDMABuf.frameActive := (sndDMABuf.frameActive + 1) and (sndDMABuf.framesCount - 1);
    inside := false;

    err := false;

    if (UseEMS) then
    begin
        err := true;
        if (emsSaveMap(SavHandle)) then
            err := false;
    end;

    fill_dmabuffer(mixbuf, @sndDMABuf);

    if (UseEMS and not err) then
        emsRestoreMap(SavHandle);
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

procedure playSetMasterVolume(vol:byte);
  begin
    if vol>127 then vol:=127;
    playState_mVolume:=vol;
    calcposttable(playState_mVolume, sdev_mode_16bit);
  end;

function playGetMasterVolume:byte;
  begin
    playGetMasterVolume:=playState_mVolume;
  end;

function playGetPatternDelay:byte;
  begin
    playGetPatternDelay:=playState_patDelayCount;
  end;

function playGetSampleRate:word;
  begin
    playGetSampleRate:=Samplerate;
  end;

procedure playSetOrder(new:boolean);
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

FUNCTION playStart(var A_stereo,A_16Bit:boolean;LQ:Boolean):boolean;
var key:boolean;
    p:parray;
    count: word;
  begin
    playStart:=false;
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

    calcposttable(playState_mVolume,A_16bit);
    (* last tick -> goto next note: *)
    playState_tick:=1;
    (* next row to read from: *)
    playState_row:=0;
    (* next order to read from: *)
    playState_order:=initState_startOrder;
    (* next pattern to read from: *)
    playState_pattern:=order[playState_order];
    (* reset pattern effects: *)
    playState_patDelayCount:=0;
    playState_patLoopActive:=false;
    playState_patLoopStartRow:=0;
    set_speed(initState_speed);
    set_tempo(initState_tempo);
    playSetOrder(playOption_ST3Order); { <- don't remove this ! it's important ! (setup lastorder) }
    playState_songEnded:=false;
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
    playStart:=true;
  end;

const
    _EM_map_name: TEMSNAME = 'saveMAP'#0;

procedure s3mplayInit;
var
    i: integer;
    _seg: word;
begin
    PROC386 := isCPU_i386;
    useEMS := emsInstalled;

  inside:=false;
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

    if (useEMS) then
    begin
        savHandle := emsAlloc(1); { 1 page is enough ? }
        if (emsEC <> E_EMS_SUCCESS) then
        begin
            Debug_Err(__FILE__, 's3mplayInit', 'Failed to allocate EM handle for mapping.');
            exit;
        end;
        emsSetHandleName(savHandle, @_EM_map_name);
    end;

    mod_Instruments := nil;
    mod_Patterns := nil;
end;

procedure s3mplayDone;
begin
    stop_play;
    player_free;
    if (useEMS) then
    begin
        emsFree(savHandle);
    end;
end;

procedure register_s3mplay; far; external;

begin
    register_s3mplay;
end.
