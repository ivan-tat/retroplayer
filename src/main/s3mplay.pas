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
function  player_init_device(input: Byte): Boolean;
procedure player_device_dump_conf;
function  player_device_get_name: PChar;
function  player_set_mode(f_16bits, f_stereo: Boolean; rate: Word; LQ: Boolean): Boolean;
function  player_get_output_rate: Word;
function  player_get_output_channels: Byte;
function  player_get_output_bits: Byte;
function  player_get_output_lq: Boolean;
procedure player_set_master_volume(value: Byte);
function  player_get_master_volume: Byte;
procedure player_set_order(extended: Boolean);
function  player_load_s3m(name: PChar): Boolean;
procedure player_set_pos;
function  player_play_start: Boolean;
procedure player_play_pause;
procedure player_play_continue;
procedure player_play_stop;
function  player_get_buffer_pos: Word;
function  player_get_speed: Byte;
function  player_get_tempo: Byte;
function  player_get_pattern_delay: Byte;
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
procedure player_device_dump_conf; external;
function  player_device_get_name: PChar; external;
function  player_set_mode(f_16bits, f_stereo: Boolean; rate: Word; LQ: Boolean): Boolean; external;
function  player_get_output_rate: Word; external;
function  player_get_output_channels: Byte; external;
function  player_get_output_bits: Byte; external;
function  player_get_output_lq: Boolean; external;
procedure player_set_master_volume(value: Byte); external;
function  player_get_master_volume: Byte; external;
procedure player_set_order(extended: Boolean); external;
function  player_load_s3m(name: PChar): Boolean; external;
procedure player_set_pos; external;
function  player_play_start: Boolean; external;
procedure player_play_pause; external;
procedure player_play_continue; external;
procedure player_play_stop; external;
function  player_get_buffer_pos: Word; external;
function  player_get_speed: Byte; external;
function  player_get_tempo: Byte; external;
function  player_get_pattern_delay: Byte; external;
procedure player_free_module; external;
procedure player_free; external;

procedure register_s3mplay; far; external;

begin
    register_s3mplay;
end.
