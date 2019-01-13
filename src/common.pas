(* common.pas -- Pascal declarations for "common" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    common;

interface

(*$I defines.pas*)

procedure __new;
procedure __delete;

procedure __copy_vmt;

function  _new_(size: Word): Pointer;
procedure _delete_(var p: Pointer);

implementation

uses
    pascal,
    i86,
    dos_;

(*$l common.obj*)

procedure __new; external;
procedure __delete; external;

procedure __copy_vmt; external;

function  _new_(size: Word): Pointer; external;
procedure _delete_(var p: Pointer); external;

end.
