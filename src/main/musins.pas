(* musins.pas -- Pascal declarations for musins.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit musins;

interface

uses
    ems;

(*$I defines.pas*)

(* Musical instrument *)

type
    TMUSINSTYPE = Byte;

const
    MUSINST_EMPTY = 0;
    MUSINST_PCM = 1;

const
    SMPFLAG_LOOP = $01;

type
    TMUSINS = array [0..16*5-1] of Byte;
    PMUSINS = ^TMUSINS;

procedure musins_clear(self: PMUSINS);
procedure musins_set_type(self: PMUSINS; value: TMUSINSTYPE);
function  musins_get_type(self: PMUSINS): TMUSINSTYPE;
procedure musins_set_looped(self: PMUSINS; value: Boolean);
function  musins_is_looped(self: PMUSINS): Boolean;
procedure musins_set_EM_data(self: PMUSINS; value: Boolean);
function  musins_is_EM_data(self: PMUSINS): Boolean;
procedure musins_set_EM_data_page(self: PMUSINS; value: Word);
function  musins_get_EM_data_page(self: PMUSINS): Word;
procedure musins_set_data(self: PMUSINS; value: Pointer);
function  musins_get_data(self: PMUSINS): Pointer;
function  musins_map_EM_data(self: PMUSINS): Pointer;
procedure musins_set_length(self: PMUSINS; value: LongInt);
function  musins_get_length(self: PMUSINS): LongInt;
procedure musins_set_loop_start(self: PMUSINS; value: LongInt);
function  musins_get_loop_start(self: PMUSINS): LongInt;
procedure musins_set_loop_end(self: PMUSINS; value: LongInt);
function  musins_get_loop_end(self: PMUSINS): LongInt;
procedure musins_set_volume(self: PMUSINS; value: Byte);
function  musins_get_volume(self: PMUSINS): Byte;
procedure musins_set_rate(self: PMUSINS; value: LongInt);
function  musins_get_rate(self: PMUSINS): LongInt;
procedure musins_set_title(self: PMUSINS; value: PChar);
function  musins_get_title(self: PMUSINS): PChar;
procedure musins_free(self: PMUSINS);

(* Musical instruments list *)

const
    MAX_INSTRUMENTS = 99;

type
    TMUSINSLIST = array [0..MAX_INSTRUMENTS-1] of TMUSINS;
    PMUSINSLIST = ^TMUSINSLIST;

function  musinsl_new: PMUSINSLIST;
procedure musinsl_clear(self: PMUSINSLIST);
procedure musinsl_set_EM_data(self: PMUSINSLIST; value: Boolean);
function  musinsl_is_EM_data(self: PMUSINSLIST): Boolean;
function  musinsl_get(self: PMUSINSLIST; index: Word): PMUSINS;
procedure musinsl_set_EM_data_handle(self: PMUSINSLIST; value: TEMSHDL);
procedure musinsl_set_EM_handle_name(self: PMUSINSLIST);
function  musinsl_get_used_EM(self: PMUSINSLIST): LongInt;
procedure musinsl_free(self: PMUSINSLIST);
procedure musinsl_delete(var self: PMUSINSLIST);

var
    mod_Instruments: PMUSINSLIST;
    InsNum: word;
    EMSSmp: boolean;
    SmpEMSHandle: TEMSHDL;

implementation

uses
    i86,
    string_,
    dos_,
    dynarray;

(*$L musins.obj*)

procedure musins_clear(self: PMUSINS); external;
procedure musins_set_type(self: PMUSINS; value: TMUSINSTYPE); external;
function  musins_get_type(self: PMUSINS): TMUSINSTYPE; external;
procedure musins_set_looped(self: PMUSINS; value: Boolean); external;
function  musins_is_looped(self: PMUSINS): Boolean; external;
procedure musins_set_EM_data(self: PMUSINS; value: Boolean); external;
function  musins_is_EM_data(self: PMUSINS): Boolean; external;
procedure musins_set_EM_data_page(self: PMUSINS; value: Word); external;
function  musins_get_EM_data_page(self: PMUSINS): Word; external;
procedure musins_set_data(self: PMUSINS; value: Pointer); external;
function  musins_get_data(self: PMUSINS): Pointer; external;
function  musins_map_EM_data(self: PMUSINS): Pointer; external;
procedure musins_set_length(self: PMUSINS; value: LongInt); external;
function  musins_get_length(self: PMUSINS): LongInt; external;
procedure musins_set_loop_start(self: PMUSINS; value: LongInt); external;
function  musins_get_loop_start(self: PMUSINS): LongInt; external;
procedure musins_set_loop_end(self: PMUSINS; value: LongInt); external;
function  musins_get_loop_end(self: PMUSINS): LongInt; external;
procedure musins_set_volume(self: PMUSINS; value: Byte); external;
function  musins_get_volume(self: PMUSINS): Byte; external;
procedure musins_set_rate(self: PMUSINS; value: LongInt); external;
function  musins_get_rate(self: PMUSINS): LongInt; external;
procedure musins_set_title(self: PMUSINS; value: PChar); external;
function  musins_get_title(self: PMUSINS): PChar; external;
procedure musins_free(self: PMUSINS); external;

function  musinsl_new: PMUSINSLIST; external;
procedure musinsl_clear(self: PMUSINSLIST); external;
procedure musinsl_set_EM_data(self: PMUSINSLIST; value: Boolean); external;
function  musinsl_is_EM_data(self: PMUSINSLIST): Boolean; external;
function  musinsl_get(self: PMUSINSLIST; index: Word): PMUSINS; external;
procedure musinsl_set_EM_data_handle(self: PMUSINSLIST; value: TEMSHDL); external;
procedure musinsl_set_EM_handle_name(self: PMUSINSLIST); external;
function  musinsl_get_used_EM(self: PMUSINSLIST): LongInt; external;
procedure musinsl_free(self: PMUSINSLIST); external;
procedure musinsl_delete(var self: PMUSINSLIST); external;

end.
