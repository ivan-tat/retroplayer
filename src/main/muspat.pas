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

procedure muspat_init;
procedure muspat_set_EM_data;
procedure muspat_is_EM_data;
procedure muspat_set_own_EM_handle;
procedure muspat_is_own_EM_handle;
procedure muspat_set_channels;
procedure muspat_get_channels;
procedure muspat_set_rows;
procedure muspat_get_rows;
procedure muspat_set_size;
procedure muspat_get_size;
procedure muspat_set_data;
procedure muspat_set_EM_data_handle;
procedure muspat_get_EM_data_handle;
procedure muspat_set_EM_data_page;
procedure muspat_get_EM_data_page;
procedure muspat_set_EM_data_offset;
procedure muspat_get_EM_data_offset;
procedure muspat_get_data;
procedure muspat_map_EM_data;

type
    PMUSPATLIST = Pointer;

procedure muspatl_init;
procedure muspatl_set;
procedure muspatl_get;
procedure muspatl_set_count;
procedure muspatl_get_count;
procedure muspatl_set_EM_data;
procedure muspatl_is_EM_data;
procedure muspatl_set_own_EM_handle;
procedure muspatl_is_own_EM_handle;
procedure muspatl_set_EM_handle;
procedure muspatl_get_EM_handle;
procedure muspatl_set_EM_handle_name;
procedure muspatl_get_used_EM;
procedure muspatl_free;

var
    mod_Patterns: PMUSPATLIST;

implementation

uses
    i86,
    string_,
    dos_,
    dynarray;

(*$L muspat.obj*)

procedure muspat_init; external;
procedure muspat_set_EM_data; external;
procedure muspat_is_EM_data; external;
procedure muspat_set_own_EM_handle; external;
procedure muspat_is_own_EM_handle; external;
procedure muspat_set_channels; external;
procedure muspat_get_channels; external;
procedure muspat_set_rows; external;
procedure muspat_get_rows; external;
procedure muspat_set_size; external;
procedure muspat_get_size; external;
procedure muspat_set_data; external;
procedure muspat_set_EM_data_handle; external;
procedure muspat_get_EM_data_handle; external;
procedure muspat_set_EM_data_page; external;
procedure muspat_get_EM_data_page; external;
procedure muspat_set_EM_data_offset; external;
procedure muspat_get_EM_data_offset; external;
procedure muspat_get_data; external;
procedure muspat_map_EM_data; external;

procedure muspatl_init; external;
procedure muspatl_set; external;
procedure muspatl_get; external;
procedure muspatl_set_count; external;
procedure muspatl_get_count; external;
procedure muspatl_set_EM_data; external;
procedure muspatl_is_EM_data; external;
procedure muspatl_set_own_EM_handle; external;
procedure muspatl_is_own_EM_handle; external;
procedure muspatl_set_EM_handle; external;
procedure muspatl_get_EM_handle; external;
procedure muspatl_set_EM_handle_name; external;
procedure muspatl_get_used_EM; external;
procedure muspatl_free; external;

end.
