(* musmodps.pas -- Pascal declarations for musmodps.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit musmodps;

interface

uses
    s3mtypes;

procedure playstate_init;
procedure playstate_alloc_channels;
procedure playstate_reset_channels;
procedure playstate_free_channels;
procedure playstate_set_speed;
procedure playstate_set_tempo;
procedure playstate_setup_patterns_order;
procedure playstate_find_next_pattern;
procedure playstate_set_pos;
procedure playstate_set_initial_state;
procedure playstate_free;

implementation

uses
    watcom,
    string_,
    common,
    dynarray,
    debug,
    muspat,
    musmod,
    mixchn;

(*$l musmodps.obj*)

procedure playstate_init; external;
procedure playstate_alloc_channels; external;
procedure playstate_reset_channels; external;
procedure playstate_free_channels; external;
procedure playstate_set_speed; external;
procedure playstate_set_tempo; external;
procedure playstate_setup_patterns_order; external;
procedure playstate_find_next_pattern; external;
procedure playstate_set_pos; external;
procedure playstate_set_initial_state; external;
procedure playstate_free; external;

end.
