(* sysdbg.pas -- Pascal declarations for "sysdbg" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    sysdbg;

interface

(*$I defines.pas*)

procedure _SYSDEBUG_LOG;
procedure _SYSDEBUG_BEGIN;
procedure _SYSDEBUG_END;
procedure _SYSDEBUG_SUCCESS;
procedure _SYSDEBUG_dump_mem;

implementation

uses
    pascal,
    string_,
    stdio,
    io,
    commdbg;

(*$L sysdbg.obj*)

procedure _SYSDEBUG_LOG; external;
procedure _SYSDEBUG_BEGIN; external;
procedure _SYSDEBUG_END; external;
procedure _SYSDEBUG_SUCCESS; external;
procedure _SYSDEBUG_dump_mem; external;

end.
