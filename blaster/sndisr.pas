(* sndisr.pas -- Pascal declarations for sndisr.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit sndisr;

interface

type
  TSoundHWISRCallback = procedure;
  PSoundHWISRCallback = ^TSoundHWISRCallback;

function  GetSoundHWISR: pointer;
procedure SetSoundHWISRCallback( p: PSoundHWISRCallback );

implementation

var
    SoundHWISRCallback: PSoundHWISRCallback;

(*$l sndisr_.obj*)
(*$l sndisr.obj*)

function  GetSoundHWISR: pointer; external;
procedure SetSoundHWISRCallback( p: PSoundHWISRCallback ); external;

end.
