(* muspat.pas -- Pascal declarations for muspat.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit muspat;

interface

uses
    ems;

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
    TMUSPATLIST = packed record
        count: Word;
        list: Pointer;
        patLength: Word;
        useEM: Boolean;
        handle: TEMSHDL;
        patPerPage: Byte;
    end;
    PMUSPATLIST = ^TMUSPATLIST;

var
    mod_Patterns: PMUSPATLIST;

procedure pat_clear(pat: PMUSPAT);
procedure patSetData(pat: PMUSPAT; p: pointer);
procedure patSetDataInEM(pat: PMUSPAT; logpage, part: byte);
function  patIsDataInEM(pat: PMUSPAT): boolean;
function  patGetData(pat: PMUSPAT): pointer;
function  patGetDataEMPage(pat: PMUSPAT): byte;
function  patGetDataEMPart(pat: PMUSPAT): byte;
function  patMapData(pat: PMUSPAT): pointer;

function  patList_new: PMUSPATLIST;
procedure patList_clear(self: PMUSPATLIST);
procedure patList_delete(var self: PMUSPATLIST);
procedure patList_set(self: PMUSPATLIST; index: integer; pat: PMUSPAT);
function  patList_get(self: PMUSPATLIST; index: integer): PMUSPAT;
function  patList_set_count(self: PMUSPATLIST; count: Word): Boolean;
function  patList_get_count(self: PMUSPATLIST): Word;
procedure patListSetPatLength(self: PMUSPATLIST; value: Word);
function  patListGetPatLength(self: PMUSPATLIST): Word;
procedure patListSetUseEM(self: PMUSPATLIST; value: Boolean);
function  patListIsInEM(self: PMUSPATLIST): Boolean;
procedure patListSetHandle(self: PMUSPATLIST; value: TEMSHDL);
function  patListGetHandle(self: PMUSPATLIST): TEMSHDL;
procedure patListSetHandleName(self: PMUSPATLIST);
procedure patListSetPatPerPage(self: PMUSPATLIST; value: Byte);
function  patListGetPatPerPage(self: PMUSPATLIST): Byte;
function  patListGetUsedEM(self: PMUSPATLIST): longint;
procedure patListFree(self: PMUSPATLIST);

implementation

uses
    i86,
    string_,
    dos_;

(*$l muspat.obj*)

procedure pat_clear(pat: PMUSPAT); external;
procedure patSetData(pat: PMUSPAT; p: pointer); external;
procedure patSetDataInEM(pat: PMUSPAT; logpage, part: byte); external;
function  patIsDataInEM(pat: PMUSPAT): boolean; external;
function  patGetData(pat: PMUSPAT): pointer; external;
function  patGetDataEMPage(pat: PMUSPAT): byte; external;
function  patGetDataEMPart(pat: PMUSPAT): byte; external;
function  patMapData(pat: PMUSPAT): pointer; external;

function  patList_new: PMUSPATLIST; external;
procedure patList_clear(self: PMUSPATLIST); external;
procedure patList_delete(var self: PMUSPATLIST); external;
procedure patList_set(self: PMUSPATLIST; index: integer; pat: PMUSPAT); external;
function  patList_get(self: PMUSPATLIST; index: integer): PMUSPAT; external;
function  patList_set_count(self: PMUSPATLIST; count: Word): Boolean; external;
function  patList_get_count(self: PMUSPATLIST): Word; external;
procedure patListSetPatLength(self: PMUSPATLIST; value: Word); external;
function  patListGetPatLength(self: PMUSPATLIST): Word; external;
procedure patListSetUseEM(self: PMUSPATLIST; value: Boolean); external;
function  patListIsInEM(self: PMUSPATLIST): Boolean; external;
procedure patListSetHandle(self: PMUSPATLIST; value: TEMSHDL); external;
function  patListGetHandle(self: PMUSPATLIST): TEMSHDL; external;
procedure patListSetHandleName(self: PMUSPATLIST); external;
procedure patListSetPatPerPage(self: PMUSPATLIST; value: Byte); external;
function  patListGetPatPerPage(self: PMUSPATLIST): Byte; external;
function  patListGetUsedEM(self: PMUSPATLIST): longint; external;
procedure patListFree(self: PMUSPATLIST); external;

end.
