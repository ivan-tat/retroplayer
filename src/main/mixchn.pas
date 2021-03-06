(* mixchn.pas -- Pascal declarations for mixchn.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixchn;

interface

(*$I defines.pas*)

procedure mixchn_init;
procedure mixchn_set_enabled;
procedure mixchn_set_playing;
procedure mixchn_set_mixing;
procedure mixchn_set_note_volume;
procedure mixchn_set_sample_period_limits;
procedure mixchn_check_sample_period;
procedure mixchn_set_sample_period;
procedure mixchn_get_sample_period;
procedure mixchn_set_sample_step;
procedure mixchn_get_sample_step;
procedure mixchn_setup_sample_period;
procedure mixchn_set_command;
procedure mixchn_get_command;
procedure mixchn_set_sub_command;
procedure mixchn_get_sub_command;
procedure mixchn_set_command_parameter;
procedure mixchn_get_command_parameter;
procedure mixchn_reset_wave_tables;
procedure mixchn_free;

procedure chn_setupInstrument;
procedure chn_calcNotePeriod;
procedure chn_calcNoteStep;
procedure chn_setupNote;

procedure __mixchnl_set_flags;

procedure mixchnl_init;
procedure mixchnl_get;
procedure mixchnl_set_count;
procedure mixchnl_get_count;
procedure mixchnl_free;

implementation

uses
    watcom,
    types,
    string_,
    dynarray,
    musins,
    musmod,
    effvars,
    mixer;

(*$l mixchn.obj*)

procedure mixchn_init; external;
procedure mixchn_set_enabled; external;
procedure mixchn_set_playing; external;
procedure mixchn_set_mixing; external;
procedure mixchn_set_note_volume; external;
procedure mixchn_set_sample_period_limits; external;
procedure mixchn_check_sample_period; external;
procedure mixchn_set_sample_period; external;
procedure mixchn_get_sample_period; external;
procedure mixchn_set_sample_step; external;
procedure mixchn_get_sample_step; external;
procedure mixchn_setup_sample_period; external;
procedure mixchn_set_command; external;
procedure mixchn_get_command; external;
procedure mixchn_set_sub_command; external;
procedure mixchn_get_sub_command; external;
procedure mixchn_set_command_parameter; external;
procedure mixchn_get_command_parameter; external;
procedure mixchn_reset_wave_tables; external;
procedure mixchn_free; external;

procedure chn_setupInstrument; external;
procedure chn_calcNotePeriod; external;
procedure chn_calcNoteStep; external;
procedure chn_setupNote; external;

procedure __mixchnl_set_flags; external;

procedure mixchnl_init; external;
procedure mixchnl_get; external;
procedure mixchnl_set_count; external;
procedure mixchnl_get_count; external;
procedure mixchnl_free; external;

end.
