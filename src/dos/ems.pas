(* ems.pas -- Pascal declarations for "ems" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit ems;

interface

(*$I defines.pas*)

type
    TEMSVER = record
        Lo, Hi: byte;
    end;

type
    TEMSERR = integer;

const
    E_EMS_SUCCESS = 0;

type
    TEMSHDL = integer;

type
    TEMSNAME = array [0..7] of char;
    PEMSNAME = ^TEMSNAME;

var
    emsInstalled: Boolean;
    emsEC: TEMSERR;
    emsVersion: TEMSVER;
    emsFrameSeg: Word;
    emsFramePtr: Pointer;

function emsIsInstalled: Boolean;
function emsGetErrorMsg: PChar;
function emsGetVersion: Boolean;
function emsGetFrameSeg: Word;
function emsGetFreePagesCount: Integer;
function emsAlloc(pages: Word): TEMSHDL;
function emsResize(handle: TEMSHDL; pages: Word): Boolean;
function emsFree(handle: TEMSHDL): Boolean;
function emsMap(handle: TEMSHDL; logPage: Word; physPage: Byte): Boolean;
function emsSaveMap(handle: TEMSHDL ): Boolean;
function emsRestoreMap(handle: TEMSHDL ): Boolean;
function emsGetHandleSize(handle: TEMSHDL): Word;
function emsSetHandleName(handle: TEMSHDL; name: PEMSNAME): Boolean;

implementation

uses
    pascal,
    i86,
    dos_,
    stdio,
    stdlib,
    string_,
    debug;

(*$l ems.obj*)
function emsIsInstalled: Boolean; external;
function emsGetErrorMsg: PChar; external;
function emsGetVersion: Boolean; external;
function emsGetFrameSeg: Word; external;
function emsGetFreePagesCount: Integer; external;
function emsAlloc(pages: Word): TEMSHDL; external;
function emsResize(handle: TEMSHDL; pages: Word): Boolean; external;
function emsFree(handle: TEMSHDL): Boolean; external;
function emsMap(handle: TEMSHDL; logPage: Word; physPage: Byte): Boolean; external;
function emsSaveMap(handle: TEMSHDL): Boolean; external;
function emsRestoreMap(handle: TEMSHDL): Boolean; external;
function emsGetHandleSize(handle: TEMSHDL): Word; external;
function emsSetHandleName(handle: TEMSHDL; name: PEMSNAME): Boolean; external;

procedure register_ems; far; external;

begin
    register_ems;
end.
