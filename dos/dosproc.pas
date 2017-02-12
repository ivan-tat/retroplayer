(* dosproc.pas -- Pascal declarations for dosproc.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit dosproc;

interface

function  getdosmem(var p; size: longint): boolean;
procedure freedosmem(var p);
function  setsize(var p; size: longint): boolean;
function  getfreesize: longint;

implementation

uses
    pascal,
    i86,
    dos_;

(*$l dosproc.obj*)
function  getdosmem(var p; size: longint): boolean; external;
procedure freedosmem(var p); external;
function  setsize(var p; size: longint): boolean; external;
function  getfreesize: longint; external;

end.
