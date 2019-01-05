(* readnote.pas -- Pascal declarations for readnote.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit readnote;

interface

(*$I defines.pas*)

procedure readnewnotes;

implementation

uses
    string_,
    ems,
    s3mtypes,
    musins,
    muspat,
    musmod,
    s3mvars,
    effvars,
    mixchn,
    effects,
    mixer;

(*$l readnote.obj*)

procedure readnewnotes; external;

end.
