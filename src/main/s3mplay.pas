{$M 16000,0,0}
{$I-,X+,V-,G+,D+}
unit s3mplay;

interface

(*$I defines.pas*)

const
    PLAYER_VERSION: PChar = '1.70.1';

procedure player_clear_error;
procedure player_is_error;
procedure player_get_error;
procedure player_init;
procedure player_init_device;
procedure player_device_dump_conf;
procedure player_device_get_name;
procedure player_set_mode;
procedure player_get_output_rate;
procedure player_get_output_channels;
procedure player_get_output_bits;
procedure player_get_output_lq;
procedure player_set_master_volume;
procedure player_get_master_volume;
procedure player_set_order;
procedure player_set_song_loop;
procedure player_load_s3m;
procedure player_set_pos;
procedure player_play_start;
procedure player_play_pause;
procedure player_play_continue;
procedure player_play_stop;
procedure player_get_buffer_pos;
procedure player_get_speed;
procedure player_get_tempo;
procedure player_get_pattern_delay;
procedure player_free_module;
procedure player_free_modules;
procedure player_free;

procedure register_s3mplay;

implementation

uses
    watcom,
    types,
    cpu,
    dos_,
    stdio,
    stdlib,
    string_,
    dynarray,
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
    fillvars,
    s3mtypes,
    musins,
    muspat,
    musmod,
    s3mvars,
    effvars,
    voltab,
    posttab,
    mixer,
    mixchn,
    filldma,
    mixing,
    effects,
    readnote;

(*$l s3mplay.obj*)

procedure player_clear_error; external;
procedure player_is_error; external;
procedure player_get_error; external;
procedure player_init; external;
procedure player_init_device; external;
procedure player_device_dump_conf; external;
procedure player_device_get_name; external;
procedure player_set_mode; external;
procedure player_get_output_rate; external;
procedure player_get_output_channels; external;
procedure player_get_output_bits; external;
procedure player_get_output_lq; external;
procedure player_set_master_volume; external;
procedure player_get_master_volume; external;
procedure player_set_order; external;
procedure player_set_song_loop; external;
procedure player_load_s3m; external;
procedure player_set_pos; external;
procedure player_play_start; external;
procedure player_play_pause; external;
procedure player_play_continue; external;
procedure player_play_stop; external;
procedure player_get_buffer_pos; external;
procedure player_get_speed; external;
procedure player_get_tempo; external;
procedure player_get_pattern_delay; external;
procedure player_free_module; external;
procedure player_free_modules; external;
procedure player_free; external;

procedure register_s3mplay; external;

end.
