(* loads3m.pas -- Pascal declarations for loads3m.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit loads3m;

interface

uses
    types,
    s3mtypes,
    muspat;

(*$I defines.pas*)

const
    S3M_MAX_PATTERNS = 100;

type
    TS3MLoader = packed record
        err: Integer;
        f: File;
        buffer: PArray;
        inspara: array [0..MAX_INSTRUMENTS-1] of Word;
        patpara: array [0..S3M_MAX_PATTERNS-1] of Word;
        smppara: array [0..MAX_INSTRUMENTS-1] of LongInt;
        pat_EM_pages: Word;
        pat_EM_page: Word;
        pat_EM_page_offset: Word;
        smp_EM_pages: Word;
        smp_EM_page: Word;
    end;
    PS3MLoader = ^TS3MLoader;

function  s3mloader_new: PS3MLoader;
procedure s3mloader_clear(self: PS3MLoader);
procedure s3mloader_delete(var self: PS3MLoader);
function  s3mloader_allocbuf(self: PS3MLoader): Boolean;
function  s3mloader_load_pattern(self: PS3MLoader; index: Byte; pos: LongInt): Boolean;
procedure s3mloader_free(self: PS3MLoader);
procedure unpackPattern(src, dst: parray; maxrow, maxchn: byte);

var
    load_error: Integer;

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
    s3mvars,
    mixchn;

(*$l loads3m.obj*)

function  s3mloader_new: PS3MLoader; external;
procedure s3mloader_clear(self: PS3MLoader); external;
procedure s3mloader_delete(var self: PS3MLoader); external;
function  s3mloader_allocbuf(self: PS3MLoader): Boolean; external;
function  s3mloader_load_pattern(self: PS3MLoader; index: Byte; pos: LongInt): Boolean; external;
procedure s3mloader_free(self: PS3MLoader); external;
procedure unpackPattern(src, dst: parray; maxrow, maxchn: byte); external;

end.
