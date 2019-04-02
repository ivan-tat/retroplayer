(* s3mvars.pas -- Pascal declarations for s3mvars.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit s3mvars;

interface

uses
    s3mtypes;

procedure playState_set_speed;
procedure playState_set_tempo;

implementation

uses
    watcom;

(*$l s3mvars.obj*)

procedure playState_set_speed; external;
procedure playState_set_tempo; external;

end.
