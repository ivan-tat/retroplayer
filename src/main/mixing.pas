(* mixing.pas -- Pascal declarations for mixing.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixing;

interface

(*$I defines.pas*)

procedure song_play;

implementation

uses
    watcom,
    string_,
    ems,
    sbctl,
    voltab,
    pcmsmp,
    musins,
    musmod,
    musmodps,
    effvars,
    effects,
    mixchn,
    mixer,
    fillvars,
    readnote;

(*$l mixing.obj*)

procedure song_play; external;

end.
