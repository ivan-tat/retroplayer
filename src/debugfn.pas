(* debugfn.pas -- Pascal declarations for "debugfn" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    debugfn;

interface

(*$I defines.pas*)

procedure _DEBUG_print(s: PChar);

procedure _DEBUG_get_xnum;
procedure _DEBUG_get_xline;
procedure _DEBUG_dump_mem;

implementation

uses
    pascal,
    string_;

procedure _DEBUG_print(s: PChar);
begin
    Write(s);
end;

(*$L debugfn.obj*)
procedure _DEBUG_get_xnum; external;
procedure _DEBUG_get_xline; external;
procedure _DEBUG_dump_mem; external;

end.
