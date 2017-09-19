(* mixer_.pas -- Pascal declarations for mixer_.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit mixer_;

interface

uses
    mixtypes;

procedure _MixSampleMono8(outBuf: pointer; var smpInfo: TPlaySampleInfo;
    volTab: word; vol: byte; count: word);
procedure _MixSampleStereo8(outBuf: pointer; var smpInfo: TPlaySampleInfo;
    volTab: word; vol: byte; count: word);

implementation

uses
    mixvars;

(*$l mixer_.obj*)
procedure _MixSampleMono8(outBuf: pointer; var smpInfo: TPlaySampleInfo;
    volTab: word; vol: byte; count: word); external;
procedure _MixSampleStereo8(outBuf: pointer; var smpInfo: TPlaySampleInfo;
    volTab: word; vol: byte; count: word); external;

end.
