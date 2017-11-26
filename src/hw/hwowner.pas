(* hwowner.pas -- Declarations for hwowner.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit hwowner;

interface

(*$I defines.pas*)

procedure hwowner_clear;
procedure hwowner_init;
procedure hwowner_free;

implementation

uses
    pascal,
    i86,
    dos_,
    string_,
    debug;

(*$l hwowner.obj*)

procedure hwowner_clear; external;
procedure hwowner_init; external;
procedure hwowner_free; external;

procedure register_hwowner; far; external;

begin
    register_hwowner;
end.
