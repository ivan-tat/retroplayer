(* muspat.pas -- Pascal declarations for muspat.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit muspat;

interface

(*$I defines.pas*)

type
    TMUSPAT = record
        data_seg: word;
            (* segment for pattern's data *)
    end;
    PMUSPAT = ^TMUSPAT;

const
    MAX_PATTERNS = 100; (* 0..99 patterns *)

type
    TPatternsArray = array [0..MAX_PATTERNS-1] of TMUSPAT;
        (* segment for every pattern *)

var
    mod_Patterns: TPatternsArray;
    patListCount: word;
    patListPatLength: word;
    patListUseEM: boolean;
    patListEMHandle: word;
    patListPatPerEMPage: byte;

procedure pat_clear(pat: PMUSPAT);
procedure patSetData(pat: PMUSPAT; p: pointer);
procedure patSetDataInEM(pat: PMUSPAT; logpage, part: byte);
function  patIsDataInEM(pat: PMUSPAT): boolean;
function  patGetData(pat: PMUSPAT): pointer;
function  patGetDataEMPage(pat: PMUSPAT): byte;
function  patGetDataEMPart(pat: PMUSPAT): byte;
function  patMapData(pat: PMUSPAT): pointer;

procedure patList_set(index: integer; pat: PMUSPAT);
function  patList_get(index: integer): PMUSPAT;
function  patListGetUsedEM: longint;
procedure patListFree;
procedure patListInit;
procedure patListDone;

implementation

uses
    i86,
    dos_,
    ems;

(*$l muspat.obj*)

procedure pat_clear(pat: PMUSPAT); external;
procedure patSetData(pat: PMUSPAT; p: pointer); external;
procedure patSetDataInEM(pat: PMUSPAT; logpage, part: byte); external;
function  patIsDataInEM(pat: PMUSPAT): boolean; external;
function  patGetData(pat: PMUSPAT): pointer; external;
function  patGetDataEMPage(pat: PMUSPAT): byte; external;
function  patGetDataEMPart(pat: PMUSPAT): byte; external;
function  patMapData(pat: PMUSPAT): pointer; external;

procedure patList_set(index: integer; pat: PMUSPAT); external;
function  patList_get(index: integer): PMUSPAT; external;
function  patListGetUsedEM: longint; external;
procedure patListFree; external;
procedure patListInit; external;
procedure patListDone; external;

end.
