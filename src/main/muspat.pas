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
    TMUSPAT = packed record
        flags: Word;
        channels: Byte;
        rows: Byte;
        size: Word;
        data_off: Word;
        data_seg: Word;
        handle: TEMSHDL;
    end;
    PMUSPAT = ^TMUSPAT;

procedure muspat_clear(self: PMUSPAT);
procedure muspat_set_EM_data(self: PMUSPAT; value: Boolean);
function  muspat_is_EM_data(self: PMUSPAT): Boolean;
procedure muspat_set_own_EM_handle(self: PMUSPAT; value: Boolean);
function  muspat_is_own_EM_handle(self: PMUSPAT): Boolean;
procedure muspat_set_channels(self: PMUSPAT; value: Byte);
function  muspat_get_channels(self: PMUSPAT): Byte;
procedure muspat_set_rows(self: PMUSPAT; value: Byte);
function  muspat_get_rows(self: PMUSPAT): Byte;
procedure muspat_set_size(self: PMUSPAT; value: Word);
function  muspat_get_size(self: PMUSPAT): Word;
procedure muspat_set_data(self: PMUSPAT; p: Pointer);
procedure muspat_set_EM_data_handle(self: PMUSPAT; value: TEMSHDL);
function  muspat_get_EM_data_handle(self: PMUSPAT): TEMSHDL;
procedure muspat_set_EM_data_page(self: PMUSPAT; value: Word);
function  muspat_get_EM_data_page(self: PMUSPAT): Word;
procedure muspat_set_EM_data_offset(self: PMUSPAT; value: Word);
function  muspat_get_EM_data_offset(self: PMUSPAT): Word;
function  muspat_get_data(self: PMUSPAT): Pointer;
function  muspat_map_EM_data(self: PMUSPAT): Pointer;

type
    PMUSPATLIST = Pointer;

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

const
    MAX_PATTERNS = 100; (* 0..99 patterns *)

var
    mod_Patterns: PMUSPATLIST;

implementation

uses
    i86,
    string_,
    dos_,
    dynarray;

(*$L muspat.obj*)

procedure muspat_clear(self: PMUSPAT); external;
procedure muspat_set_EM_data(self: PMUSPAT; value: Boolean); external;
function  muspat_is_EM_data(self: PMUSPAT): Boolean; external;
procedure muspat_set_own_EM_handle(self: PMUSPAT; value: Boolean); external;
function  muspat_is_own_EM_handle(self: PMUSPAT): Boolean; external;
procedure muspat_set_channels(self: PMUSPAT; value: Byte); external;
function  muspat_get_channels(self: PMUSPAT): Byte; external;
procedure muspat_set_rows(self: PMUSPAT; value: Byte); external;
function  muspat_get_rows(self: PMUSPAT): Byte; external;
procedure muspat_set_size(self: PMUSPAT; value: Word); external;
function  muspat_get_size(self: PMUSPAT): Word; external;
procedure muspat_set_data(self: PMUSPAT; p: Pointer); external;
procedure muspat_set_EM_data_handle(self: PMUSPAT; value: TEMSHDL); external;
function  muspat_get_EM_data_handle(self: PMUSPAT): TEMSHDL; external;
procedure muspat_set_EM_data_page(self: PMUSPAT; value: Word); external;
function  muspat_get_EM_data_page(self: PMUSPAT): Word; external;
procedure muspat_set_EM_data_offset(self: PMUSPAT; value: Word); external;
function  muspat_get_EM_data_offset(self: PMUSPAT): Word; external;
function  muspat_get_data(self: PMUSPAT): Pointer; external;
function  muspat_map_EM_data(self: PMUSPAT): Pointer; external;

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
