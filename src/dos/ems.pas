(* ems.pas -- Pascal declarations for "ems" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit ems;

interface

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)

type
    TEMSVER = packed record
        Lo, Hi: byte;
    end;

var
    emsInstalled: Boolean;
    emsEC: Integer;
    emsVersion: TEMSVER;
    emsFrameSeg: Word;
    emsFramePtr: Pointer;

(*$endif*)  (* DEFINE_LOCAL_DATA *)

procedure emsIsInstalled;
procedure emsGetErrorMsg;
procedure emsGetVersion;
procedure emsGetFrameSeg;
procedure emsGetFreePagesCount;
procedure emsAlloc;
procedure emsResize;
procedure emsFree;
procedure emsMap;
procedure emsSaveMap;
procedure emsRestoreMap;
procedure emsGetHandleSize;
procedure emsSetHandleName;

procedure register_ems;

implementation

uses
    pascal,
    i86,
    dos_,
    stdio,
    stdlib,
    string_,
    common,
    debug;

(*$l ems.obj*)
procedure emsIsInstalled; external;
procedure emsGetErrorMsg; external;
procedure emsGetVersion; external;
procedure emsGetFrameSeg; external;
procedure emsGetFreePagesCount; external;
procedure emsAlloc; external;
procedure emsResize; external;
procedure emsFree; external;
procedure emsMap; external;
procedure emsSaveMap; external;
procedure emsRestoreMap; external;
procedure emsGetHandleSize; external;
procedure emsSetHandleName; external;

procedure register_ems; external;

end.
