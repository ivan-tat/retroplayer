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
procedure playState_set_speed;
procedure playState_set_tempo;
procedure playstate_free;

implementation

uses
    watcom,
    string_;

(*$l musmodps.obj*)

procedure playstate_init; external;
procedure playState_set_speed; external;
procedure playState_set_tempo; external;
procedure playstate_free; external;

end.
