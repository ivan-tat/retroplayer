(* emstool.pas -- Pascal declarations for emstool.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit EMStool;

interface

(*$I defines.pas*)

type
    TEMMVersion = record
        Lo, Hi: byte;
    end;
    TEMMError = integer;
    TEMMHandle = integer;
    TEMMHandleName = array [0..7] of char;
    PEMMHandleName = ^TEMMHandleName;

var
    EmsInstalled: boolean;
    EmsEC: TEMMError;
    EmsVersion: TEMMVersion;
    FrameSEG: array[0..3] of word; (* real mem segment for every page *)
    FramePTR: array[0..3] of pointer; (* real mem pointer (ofs=0) for every page *)

function CheckEMM: boolean;
function GetEMMVersion: boolean;
function GetEMMFrameSeg: word;
function EmsFreePages: integer;
function EmsAlloc( pages: word ): TEMMHandle;
function EmsFree( handle: TEMMHandle ): boolean;
function EmsMap( handle: TEMMHandle; logPage: word; physPage: byte ): boolean;
function EmsSaveMap( handle: TEMMHandle ): boolean;
function EmsRestoreMap( handle: TEMMHandle ): boolean;
function EmsGetHandleSize( handle: TEMMHandle ): word;
function EmsSetHandleName( handle: TEMMHandle; name: TEMMHandleName ): boolean;

implementation

uses
    pascal,
    malloc,
    stdio,
    stdlib,
    string_,
    i86;

(*$l emstool.obj*)
function CheckEMM: boolean; external;
function GetEMMErrorMsg( err: TEMMError ): PChar; external;
function GetEMMVersion: boolean; external;
function GetEMMFrameSeg: word; external;
function EmsFreePages: integer; external;
function EmsAlloc( pages: word ): TEMMHandle; external;
function EmsFree( handle: TEMMHandle ): boolean; external;
function EmsMap( handle: TEMMHandle; logPage: word; physPage: byte ): boolean; external;
function EmsSaveMap( handle: TEMMHandle ): boolean; external;
function EmsRestoreMap( handle: TEMMHandle ): boolean; external;
function EmsGetHandleSize( handle: TEMMHandle ): word; external;
function EmsSetHandleName( handle: TEMMHandle; name: TEMMHandleName ): boolean; external;

procedure register_emstool; far; external;
procedure unregister_emstool; far; external;

begin
    register_emstool;
end.
