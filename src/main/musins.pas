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

procedure musins_init;
procedure musins_set_type;
procedure musins_get_type;
procedure musins_set_looped;
procedure musins_is_looped;
procedure musins_set_EM_data;
procedure musins_is_EM_data;
procedure musins_set_EM_data_page;
procedure musins_get_EM_data_page;
procedure musins_set_data;
procedure musins_get_data;
procedure musins_map_EM_data;
procedure musins_set_length;
procedure musins_get_length;
procedure musins_set_loop_start;
procedure musins_get_loop_start;
procedure musins_set_loop_end;
procedure musins_get_loop_end;
procedure musins_set_volume;
procedure musins_get_volume;
procedure musins_set_rate;
procedure musins_get_rate;
procedure musins_set_title;
procedure musins_get_title;
procedure musins_free;

(* Musical instruments list *)

const
    MAX_INSTRUMENTS = 99;

type
    TMUSINSLIST = array [0..MAX_INSTRUMENTS-1] of TMUSINS;
    PMUSINSLIST = ^TMUSINSLIST;

procedure musinsl_new;
procedure musinsl_init;
procedure musinsl_set_EM_data;
procedure musinsl_is_EM_data;
procedure musinsl_get;
procedure musinsl_set_EM_data_handle;
procedure musinsl_set_EM_handle_name;
procedure musinsl_get_used_EM;
procedure musinsl_free;
procedure musinsl_delete;

var
    mod_Instruments: PMUSINSLIST;
    mod_InstrumentsCount: word;
    EMSSmp: boolean;
    SmpEMSHandle: TEMSHDL;

implementation

uses
    i86,
    string_,
    dos_,
    dynarray;

(*$L musins.obj*)

procedure musins_init; external;
procedure musins_set_type; external;
procedure musins_get_type; external;
procedure musins_set_looped; external;
procedure musins_is_looped; external;
procedure musins_set_EM_data; external;
procedure musins_is_EM_data; external;
procedure musins_set_EM_data_page; external;
procedure musins_get_EM_data_page; external;
procedure musins_set_data; external;
procedure musins_get_data; external;
procedure musins_map_EM_data; external;
procedure musins_set_length; external;
procedure musins_get_length; external;
procedure musins_set_loop_start; external;
procedure musins_get_loop_start; external;
procedure musins_set_loop_end; external;
procedure musins_get_loop_end; external;
procedure musins_set_volume; external;
procedure musins_get_volume; external;
procedure musins_set_rate; external;
procedure musins_get_rate; external;
procedure musins_set_title; external;
procedure musins_get_title; external;
procedure musins_free; external;

procedure musinsl_new; external;
procedure musinsl_init; external;
procedure musinsl_set_EM_data; external;
procedure musinsl_is_EM_data; external;
procedure musinsl_get; external;
procedure musinsl_set_EM_data_handle; external;
procedure musinsl_set_EM_handle_name; external;
procedure musinsl_get_used_EM; external;
procedure musinsl_free; external;
procedure musinsl_delete; external;

end.
