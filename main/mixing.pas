(* mixing.pas -- Pascal declarations for mixing.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixing;

interface

procedure calc_tick; (* internal use *)

implementation

uses
    emstool,
    sbctl,
    voltab,
    s3mvars,
    effvars,
    mixvars,
    fillvars,
    mixer,
    mixer_,
    effects,
    readnote;

(*$l mixing.obj*)
procedure calc_tick; external;

end.
