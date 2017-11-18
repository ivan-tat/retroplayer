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

procedure player_clear_error;
function  player_is_error: Boolean;
function  player_get_error: PChar;
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

procedure player_clear_error; external;
function  player_is_error: Boolean; external;
function  player_get_error: PChar; external;
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
