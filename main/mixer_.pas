(* mixer_.pas -- Pascal declarations for mixer_.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixer_;

interface

function  _mixCalcSampleStep(wPeriod: word): longint;
procedure _MixSampleMono8;
procedure _MixSampleStereo8;

implementation

uses
    mixvars;

(*$l mixer_.obj*)
function  _mixCalcSampleStep(wPeriod: word): longint; external;
procedure _MixSampleMono8; external;
procedure _MixSampleStereo8; external;

end.
