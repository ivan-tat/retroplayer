(* malloc.pas -- Pascal declarations for malloc.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit malloc;

interface

(*$I defines.pas*)

function _memmax: longint;

implementation

uses
    pascal,
    dos_;

(*$l malloc.obj*)
function _memmax: longint; external;

end.
