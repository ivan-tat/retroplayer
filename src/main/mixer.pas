(* mixer.pas -- Pascal declarations for mixer.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixer;

interface

(*$I defines.pas*)

function  _calc_sample_step(wPeriod: word): longint;

implementation

uses
    watcom,
    mixvars;

(*$l mixer.obj*)

function  _calc_sample_step(wPeriod: word): longint; external;

end.
