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

function  muspatl_new: PMUSPATLIST;
procedure muspatl_clear(self: PMUSPATLIST);
procedure muspatl_delete(var self: PMUSPATLIST);
procedure muspatl_set(self: PMUSPATLIST; index: Word; item: PMUSPAT);
function  muspatl_get(self: PMUSPATLIST; index: Word): PMUSPAT;
function  muspatl_set_count(self: PMUSPATLIST; value: Word): Boolean;
function  muspatl_get_count(self: PMUSPATLIST): Word;
procedure muspatl_set_EM_data(self: PMUSPATLIST; value: Boolean);
function  muspatl_is_EM_data(self: PMUSPATLIST): Boolean;
procedure muspatl_set_own_EM_handle(self: PMUSPATLIST; value: Boolean);
function  muspatl_is_own_EM_handle(self: PMUSPATLIST): Boolean;
procedure muspatl_set_EM_handle(self: PMUSPATLIST; value: TEMSHDL);
function  muspatl_get_EM_handle(self: PMUSPATLIST): TEMSHDL;
procedure muspatl_set_EM_handle_name(self: PMUSPATLIST);
function  muspatl_get_used_EM(self: PMUSPATLIST): longint;
procedure muspatl_free(self: PMUSPATLIST);

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

function  muspatl_new: PMUSPATLIST; external;
procedure muspatl_clear(self: PMUSPATLIST); external;
procedure muspatl_delete(var self: PMUSPATLIST); external;
procedure muspatl_set(self: PMUSPATLIST; index: Word; item: PMUSPAT); external;
function  muspatl_get(self: PMUSPATLIST; index: Word): PMUSPAT; external;
function  muspatl_set_count(self: PMUSPATLIST; value: Word): Boolean; external;
function  muspatl_get_count(self: PMUSPATLIST): Word; external;
procedure muspatl_set_EM_data(self: PMUSPATLIST; value: Boolean); external;
function  muspatl_is_EM_data(self: PMUSPATLIST): Boolean; external;
procedure muspatl_set_own_EM_handle(self: PMUSPATLIST; value: Boolean); external;
function  muspatl_is_own_EM_handle(self: PMUSPATLIST): Boolean; external;
procedure muspatl_set_EM_handle(self: PMUSPATLIST; value: TEMSHDL); external;
function  muspatl_get_EM_handle(self: PMUSPATLIST): TEMSHDL; external;
procedure muspatl_set_EM_handle_name(self: PMUSPATLIST); external;
function  muspatl_get_used_EM(self: PMUSPATLIST): longint; external;
procedure muspatl_free(self: PMUSPATLIST); external;

end.
