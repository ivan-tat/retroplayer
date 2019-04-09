{$M 16000,0,0}
{$I-,X+,V-,G+,D+}
unit s3mplay;

interface

(*$I defines.pas*)

const
    PLAYER_VERSION: PChar = '1.70.1';

procedure player_new;
procedure player_init;
procedure player_is_error;
procedure player_get_error;
procedure player_set_EM_usage;
procedure player_is_EM_in_use;

procedure player_init_device;
procedure player_device_get_name;
procedure player_device_dump_conf;
procedure player_get_sound_buffer;
procedure player_set_sound_buffer_fps;
procedure player_set_mode;
procedure player_get_output_rate;
procedure player_get_output_channels;
procedure player_get_output_bits;
procedure player_get_output_lq;
procedure player_play_start;
procedure player_play_pause;
procedure player_play_continue;
procedure player_play_stop;
procedure player_get_buffer_pos;
procedure player_free_device;

procedure player_get_mixer;

procedure player_load_s3m;
procedure player_set_active_track;
procedure player_get_play_state;
procedure player_set_master_volume;
procedure player_get_master_volume;
procedure player_free_module;
procedure player_free_modules;

procedure player_free;
procedure player_delete;

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
    s3mtypes,
    musins,
    muspat,
    musmod,
    mixchn,
    musmodps,
    effvars,
    effects,
    voltab,
    posttab,
    readnote,
    mixer,
    mixing,
    fillvars,
    filldma;

(*$l s3mplay.obj*)

procedure player_new; external;
procedure player_init; external;
procedure player_is_error; external;
procedure player_get_error; external;
procedure player_set_EM_usage; external;
procedure player_is_EM_in_use; external;

procedure player_init_device; external;
procedure player_device_get_name; external;
procedure player_device_dump_conf; external;
procedure player_get_sound_buffer; external;
procedure player_set_sound_buffer_fps; external;
procedure player_set_mode; external;
procedure player_get_output_rate; external;
procedure player_get_output_channels; external;
procedure player_get_output_bits; external;
procedure player_get_output_lq; external;
procedure player_play_start; external;
procedure player_play_pause; external;
procedure player_play_continue; external;
procedure player_play_stop; external;
procedure player_get_buffer_pos; external;
procedure player_free_device; external;

procedure player_get_mixer; external;

procedure player_load_s3m; external;
procedure player_set_active_track; external;
procedure player_get_play_state; external;
procedure player_set_master_volume; external;
procedure player_get_master_volume; external;
procedure player_free_module; external;
procedure player_free_modules; external;

procedure player_free; external;
procedure player_delete; external;

procedure register_s3mplay; external;

end.
