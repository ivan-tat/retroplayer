(* loads3m.pas -- Pascal declarations for loads3m.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit loads3m;

interface

(*$I defines.pas*)

type
    PS3MLoader = Pointer;

function  s3mloader_new: PS3MLoader;
procedure s3mloader_init(self: PS3MLoader);
function  s3mloader_load(self: PS3MLoader; name: PChar): Boolean;
function  s3mloader_get_error(self: PS3MLoader): PChar;
procedure s3mloader_free(self: PS3MLoader);
procedure s3mloader_delete(var self: PS3MLoader);

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

function  s3mloader_new: PS3MLoader; external;
procedure s3mloader_init(self: PS3MLoader); external;
function  s3mloader_load(self: PS3MLoader; name: PChar): Boolean; external;
function  s3mloader_get_error(self: PS3MLoader): PChar; external;
procedure s3mloader_free(self: PS3MLoader); external;
procedure s3mloader_delete(var self: PS3MLoader); external;

end.
