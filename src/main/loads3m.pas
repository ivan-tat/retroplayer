(* loads3m.pas -- Pascal declarations for loads3m.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit loads3m;

interface

(*$I defines.pas*)

procedure s3mloader_new;
procedure s3mloader_init;
procedure s3mloader_load;
procedure s3mloader_get_error;
procedure s3mloader_free;
procedure s3mloader_delete;

implementation

uses
    stdio,
    string_,
    debug,
    strutils,
    crt,
    dos,
    dos_,
    ems,
    common,
    musins,
    muspat,
    s3mtypes,
    s3mvars,
    mixchn;

(*$l loads3m.obj*)

procedure s3mloader_new; external;
procedure s3mloader_init; external;
procedure s3mloader_load; external;
procedure s3mloader_get_error; external;
procedure s3mloader_free; external;
procedure s3mloader_delete; external;

end.
