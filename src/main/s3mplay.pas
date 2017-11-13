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
      Allreadyallocbuffers = -6; { don't try to allocate memory for buffers twice }
      nota386orhigher      = -7; { for playing any sound we need a 386 or higher -
                                   sorry but I optimized it for a 486 (pipeline etc.) and it
                                   runs fine on a 386 ;)
                                   (Hey guys a 486DX is not that expensive - for
                                   the same price I got an slow 386SX in 1991) }
      nosounddevice        = -8; { before 'start playing' - set a sounddevice ! }
      noS3Minmemory        = -9; { before 'start playing' - load a S3M ! }
      internal_failure     = -11; { I'm sorry if this happend :( }
      E_failed_to_load_file = -100;

{ variables for public }
VAR
    player_Error: Integer;
    player_Error_msg: PChar;

function  player_load_s3m(name: String): Boolean;
    (* load S3M module into memory *)

procedure player_free_module;
    (* free memory used by module *)

function  player_init_device(input: Byte): Boolean;

function  player_init: Boolean;
    (* init DMABuf,mixBuf,volumetable and some variables *)

procedure player_free;
    (* free buffers used by player *)

function  player_set_mode(f_16bits, f_stereo: Boolean; rate: Word; LQ: Boolean): Boolean;
function  playStart: Boolean;
procedure playSetMasterVolume(vol: Byte);
procedure playSetOrder(new: Boolean);   (* look at ST3order *)
function  playGetSpeed: byte;
function  playGetTempo: byte;
function  playGetMasterVolume: byte;
function  playGetPatternDelay: byte;
function  playGetSampleRate: word;
function  player_is_lq_mode: Boolean;

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
    common,
    loads3m,
    pic,
    sndctl_t,
    sbctl,
    effvars,
    mixer,
    mixchn,
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
    player_mode_set: Boolean;
    player_mode_bits: Byte;
    player_mode_signed: Boolean;
    player_mode_channels: Byte;
    player_mode_rate: Word;
    player_mode_lq: Boolean;
    oldexitproc: Pointer;

const
    _EM_map_name: TEMSNAME = 'saveMAP'#0;

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
        _delete_(mod_Patterns);
    end;

    mod_isLoaded := false;
end;

PROCEDURE player_free;
begin
    stop_play;
    player_free_module;
    restore_irq;
    freeVolumeTable;
    snddmabuf_free(@sndDMABuf);
    if (mixBuf <> Nil) then
    begin
        _dos_freemem(seg(mixBuf^));
        mixBuf := nil;
    end;
    buffersreserved:=false;
    if (useEMS) then
    begin
        emsFree(SavHandle);
    end;
end;

function player_load_s3m(name: String): Boolean;
var
    p: PS3MLoader;
    cname: array [0..255] of Char;
begin
    p := s3mloader_new;
    if (p = nil) then
    begin
        player_error := E_failed_to_load_file;
        player_error_msg := 'Failed to initialize S3M loader.';
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to initialize S3M loader.');
        player_load_s3m := false;
        exit;
    end;
    s3mloader_init(p);

    memcpy(cname, name[1], Ord(name[0]));
    cname[Ord(name[0])] := Chr(0);

    if (not s3mloader_load(p, cname)) then
    begin
        player_error := E_failed_to_load_file;
        player_error_msg := s3mloader_get_error(p);
        Debug_Err(__FILE__, 'player_load_s3m', 'Failed to load S3M file.');
        s3mloader_free(p);
        s3mloader_delete(p);
        player_free_module;
        player_load_s3m := false;
        exit;
    end;

    s3mloader_free(p);
    s3mloader_delete(p);

    player_load_s3m := true;
end;

FUNCTION player_init_device(input:byte):boolean;
  begin
    player_init_device:=false;
    if Input = 0 then { 'checkthem' not yet implemented } sounddevice:=true
    else
    if Input = 1 then Sounddevice:=DetectSoundblaster
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
    DEBUG_BEGIN(__FILE__, 'player_init');
    player_init:=false;
    if not proc386 then begin player_error:=nota386orhigher;exit end;
    if (useEMS) then
    begin
        SavHandle := emsAlloc(1); { 1 page is enough ? }
        if (emsEC <> E_EMS_SUCCESS) then
        begin
            Debug_Err(__FILE__, 's3mplayInit', 'Failed to allocate EM handle for mapping.');
            exit;
        end;
        emsSetHandleName(SavHandle, @_EM_map_name);
    end;
    if buffersreserved then begin player_error:=Allreadyallocbuffers;player_init:=true;exit end;
    if ( not allocVolumeTable ) then begin player_error:=notenoughmem;exit end;

    if (not snddmabuf_alloc(@sndDMABuf, DMA_BUF_SIZE_MAX)) then
    begin
        player_error := notenoughmem;
        exit;
    end;
    {
      in tick buffer we calc one DMA buffer half - that are DMA_BUF_SIZE_MAX/2 words
    }
    if (_dos_allocmem((sndDMABuf.buf^.Size + 15) shr 4, _seg) <> 0) then
    begin
        snddmabuf_free(@sndDMABuf);
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

function _player_setup_mixer: Boolean;
var
    tmp: Word;
    rate: Word;
begin
    DEBUG_BEGIN(__FILE__, '_player_setup_mixer');

    if (player_mode_set) then
    begin
        rate := player_mode_rate;

        if (player_mode_lq) then
            rate := rate div 2;

        (*
        tmp := longint(1000000) div rate;
        setMixMode(
            player_mode_channels,
            rate,
            ((longint(1000000) div tmp) div playOption_FPS) + 1
        );
        *)
        setMixMode(
            player_mode_channels,
            rate,
            trunc(trunc(1000000 / trunc(1000000 / rate)) / playOption_FPS) + 1
        );
        DEBUG_SUCCESS(__FILE__, '_player_setup_mixer');
        _player_setup_mixer := true;
    end
    else
    begin
        DEBUG_FAIL(__FILE__, '_player_setup_mixer', 'No play mode was set.');
        _player_setup_mixer := false;
    end;
end;

function _player_setup_outbuf(outbuf: PSNDDMABUF; spc: Word): Boolean;
var
    size: Word;
    i, count: Byte;
begin
    DEBUG_BEGIN(__FILE__, '_player_setup_outbuf');

    if (player_mode_set) then
    begin
        outbuf^.flags := 0;

        if (player_mode_lq) then
            outbuf^.flags := outbuf^.flags or SNDDMABUFFL_LQ;

        set_sample_format(@(outbuf^.format),
            player_mode_bits, player_mode_signed, player_mode_channels);

        size := spc;
        if (player_mode_bits = 16) then
            size := size shl 1;

        if (player_mode_channels = 2) then
            size := size shl 1;

        outbuf^.frameSize := size;

        i := outbuf^.buf^.Size div size;
        count := 1;
        while (count < i) do
            count := count shl 1;

        if (player_mode_lq) then
            count := count shr 1;

        outbuf^.framesCount := count shr 1;
        DEBUG_SUCCESS(__FILE__, '_player_setup_outbuf');
        _player_setup_outbuf := true;
    end
    else
    begin
        DEBUG_FAIL(__FILE__, '_player_setup_outbuf', 'No play mode was set.');
        _player_setup_outbuf := false;
    end;
end;

function player_set_mode(f_16bits, f_stereo: Boolean; rate: Word; LQ: Boolean): Boolean;
begin
    DEBUG_BEGIN(__FILE__, 'player_set_mode');

    case f_16bits of
        false:
            begin
                player_mode_bits := 8;
                player_mode_signed := false;
            end;
        true:
            begin
                player_mode_bits := 16;
                player_mode_signed := true;
            end;
    end;

    case f_stereo of
        false:
            player_mode_channels := 1;
        true:
            player_mode_channels := 2;
    end;

    player_mode_rate := rate;
    player_mode_lq := LQ;
    player_mode_set := true;

    DEBUG_SUCCESS(__FILE__, 'player_set_mode');
    player_set_mode := true;
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
    calcposttable(playState_mVolume, sdev_mode_16bits);
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
    playGetSampleRate:=player_mode_rate;
  end;

function player_is_lq_mode: Boolean;
begin
    player_is_lq_mode := player_mode_lq;
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

function playStart: Boolean;
var
    outbuf: PSNDDMABUF;
    count: Word;
    mode_stereo: Boolean;
    mode_16bits: Boolean;
begin
    DEBUG_BEGIN(__FILE__, 'playStart');

    player_error := 0;
    player_error_msg := nil;

    if (not sounddevice) then
    begin
        DEBUG_FAIL(__FILE__, 'playStart', 'No sound device was set.');
        player_error := nosounddevice;
        playStart := false;
        exit;
    end;

    if (not player_mode_set) then
    begin
        DEBUG_FAIL(__FILE__, 'playStart', 'No play mode was set.');
        player_error := internal_failure;
        playStart := false;
        exit;
    end;

    if (not mod_isLoaded) then
    begin
        DEBUG_FAIL(__FILE__, 'playStart', 'No music module was loaded.');
        player_error := noS3Minmemory;
        playStart := false;
        exit;
    end;

    mode_16bits := player_mode_bits = 16;
    mode_stereo := player_mode_channels = 2;

    sbAdjustMode(player_mode_rate, mode_stereo, mode_16bits);

    set_ready_irq( @PlaySoundCallback );
    Initblaster(mode_16bits, mode_stereo, player_mode_rate);

    if (mode_16bits) then
        player_mode_bits := 16
    else
        player_mode_bits := 8;
    if (mode_stereo) then
        player_mode_channels := 2
    else
        player_mode_channels := 1;

    player_set_mode(mode_16bits, mode_stereo, player_mode_rate, player_mode_lq);

    if (not _player_setup_mixer) then
    begin
        DEBUG_FAIL(__FILE__, 'playStart', 'Failed to setup mixer.');
        playStart := false;
        exit;
    end;

    outbuf := @sndDMABuf;

    if (not _player_setup_outbuf(outbuf, mixBufSamplesPerChannel)) then
    begin
        DEBUG_FAIL(__FILE__, 'playStart', 'Failed to setup output buffer.');
        playStart := false;
        exit;
    end;

    (* now after loading we know if signed data or not *)
    calcVolumeTable( modOption_SignedData );

    calcposttable(playState_mVolume,mode_16bits);
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

    mixTickSamplesPerChannelLeft:=0;    (* emmidiately next tick *)
    Initchannels;

    count := outbuf^.frameSize;
    if (player_mode_lq) then
        count := count * 2;

    outbuf^.frameActive := outbuf^.framesCount - 1;
    outbuf^.frameLast := outbuf^.framesCount;

    (* calc all buffer parts *)
    fill_dmabuffer(mixbuf, outbuf);

    (* loop through whole DMA buffer with double buffering *)
    sbSetupDMATransfer(outbuf^.buf^.Data, count * outbuf^.framesCount, true);
    sbSetupDSPTransfer(count, true);

    (* ok, now everything works in background *)
    DEBUG_SUCCESS(__FILE__, 'playStart');
    playStart:=true;
end;

procedure s3mplayInit;
var
    i: integer;
    _seg: word;
begin
    PROC386 := isCPU_i386;
    useEMS := emsInstalled;
    inside := false;
    buffersreserved := false;
    sounddevice := false;
    player_mode_set := false;
    player_mode_bits := 0;
    player_mode_signed := false;
    player_mode_channels := 0;
    player_mode_rate := 0;
    player_mode_lq := false;
    playOption_LoopSong := false;
    playOption_ST3Order := false;   { Ok let's hear all patterns are saved ... }
    playOption_FPS := 70;
    initVolumeTable;
    mixBuf := nil;  (* FIXME: initMixBuf() *)
    mixSampleRate := 0;
    mixChannels := 0;
    mixBufSamplesPerChannel := 0;
    snddmabuf_init(@sndDMABuf);
    SavHandle := EMSBADHDL;
    mod_Instruments := nil;
    mod_Patterns := nil;
end;

procedure s3mplayDone;
begin
    player_free;
end;

procedure register_s3mplay; far; external;

begin
    register_s3mplay;
end.
