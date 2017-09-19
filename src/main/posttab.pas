(* posttab.pas -- Pascal declarations for posttab.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit posttab;

interface

(*$I defines.pas*)

var
    post8bit :array[0..4095] of byte;
    post16bit:array[0..4095] of word;

procedure calcPostTable( vol: byte; use16bit: boolean );

implementation

uses i4m;

(*$l posttab.obj*)

procedure calcPostTable( vol: byte; use16bit: boolean ); external;

end.
