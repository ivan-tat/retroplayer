(* Pascal declarations for mixer.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit mixer;

interface

function  mixCalcSampleStep(period: word): longint;

implementation

uses
    i4m,
    i8d086,
    mixvars;

(*$l mixer.obj*)
function  mixCalcSampleStep(period: word): longint; external;

end.
