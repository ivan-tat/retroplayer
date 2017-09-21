(* loads3m.pas -- Pascal declarations for loads3m.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit loads3m;

interface

uses
    types,
    s3mtypes;

(*$I defines.pas*)

procedure unpackPattern(src, dst: parray; maxrow, maxchn: byte);

implementation

uses
    stdio,
    string_,
    strutils,
    crt,
    dos,
    dos_,
    emstool,
    s3mvars;

(*$l loads3m.obj*)

procedure unpackPattern(src, dst: parray; maxrow, maxchn: byte); external;

end.