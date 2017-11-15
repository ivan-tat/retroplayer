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

function  player_init: Boolean;
    (* init DMABuf,mixBuf,volumetable and some variables *)
function  player_init_device(input: Byte): Boolean;
function  player_set_mode(f_16bits, f_stereo: Boolean; rate: Word; LQ: Boolean): Boolean;
function  playGetSampleRate: Word;
function  player_is_lq_mode: Boolean;
procedure playSetMasterVolume(value: Byte);
function  playGetMasterVolume: Byte;
procedure playSetOrder(extended: Boolean);
function  player_load_s3m(name: PChar): Boolean;
function  playStart: Boolean;
function  playGetSpeed: Byte;
function  playGetTempo: Byte;
function  playGetPatternDelay: Byte;
procedure player_free_module;
procedure player_free;

IMPLEMENTATION

uses
    watcom,
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

(*$l s3mplay.obj*)

function  player_init: Boolean; external;
function  player_init_device(input: Byte): Boolean; external;
function  player_set_mode(f_16bits, f_stereo: Boolean; rate: Word; LQ: Boolean): Boolean; external;
function  playGetSampleRate: Word; external;
function  player_is_lq_mode: Boolean; external;
procedure playSetMasterVolume(value: Byte); external;
function  playGetMasterVolume: Byte; external;
procedure playSetOrder(extended: Boolean); external;
function  player_load_s3m(name: PChar): Boolean; external;
function  playStart: Boolean; external;
function  playGetSpeed: Byte; external;
function  playGetTempo: Byte; external;
function  playGetPatternDelay: Byte; external;
procedure player_free_module; external;
procedure player_free; external;

procedure register_s3mplay; far; external;

begin
    register_s3mplay;
end.
