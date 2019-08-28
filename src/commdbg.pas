(* commdbg.pas -- Pascal declarations for "commdbg" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    commdbg;

interface

(*$I defines.pas*)

procedure _DEBUG_get_xnum;
procedure _DEBUG_get_xline;

implementation

uses
    pascal,
    string_;

(*$L commdbg.obj*)
procedure _DEBUG_get_xnum; external;
procedure _DEBUG_get_xline; external;

end.
