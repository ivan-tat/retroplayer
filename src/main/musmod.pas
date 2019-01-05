(* musmod.pas -- Pascal declarations for musmod.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit musmod;

interface

(*$I defines.pas*)

const
    MUSMOD_CHANNELS_MAX = 32;

procedure __musmod_set_flags;

procedure musmod_init;
procedure musmod_set_title;
procedure musmod_set_format;
procedure musmod_free;

var
    mod_Track: Pointer;

implementation

uses
    common,
    string_,
(*
    pcmsmp,
*)
    musins,
    muspat;

(*$l musmod.obj*)

procedure __musmod_set_flags; external;

procedure musmod_init; external;
procedure musmod_set_title; external;
procedure musmod_set_format; external;
procedure musmod_free; external;

end.
