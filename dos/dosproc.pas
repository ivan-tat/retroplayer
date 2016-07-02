(* Declarations for dosproc.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit dosproc;

interface

function getdosmem(var p; size: longint): boolean;
procedure freedosmem(var p);
function getfreesize: word;
function setsize(var p; size: longint): boolean;

implementation

(*$l dosproc.obj*)

function getdosmem(var p; size: longint): boolean; external;
procedure freedosmem(var p); external;
function getfreesize: word; external;
function setsize(var p; size: longint): boolean; external;

end.
