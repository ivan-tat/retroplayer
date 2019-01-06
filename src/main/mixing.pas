(* mixing.pas -- Pascal declarations for mixing.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixing;

interface

(*$I defines.pas*)

procedure sound_fill_buffer;

implementation

uses
    string_,
    ems,
    sbctl,
    voltab,
    pcmsmp,
    musins,
    s3mvars,
    effvars,
    fillvars,
    mixer,
    mixchn,
    effects,
    readnote;

(*$l mixing.obj*)

procedure sound_fill_buffer; external;

end.
