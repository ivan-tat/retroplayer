(* unistd.pas -- Pascal declarations for custom "unistd" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    unistd;

interface

(*$I defines.pas*)

procedure cc_execv;

implementation

uses
    pascal,
    string_,
    errno_,
    i86,
    sysdbg,
    startup,
    dos_;

(*$L unistd\exv.obj*)
procedure cc_execv; external;

end.
