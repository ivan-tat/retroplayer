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

procedure muspatchnevent_clear;
procedure muspatrowevent_clear;

procedure __muspat_set_flags;

procedure muspat_init;
procedure muspat_get_data;
procedure muspat_map_EM_data;
procedure muspat_set_packed_row_start;
procedure muspat_get_packed_row_start;
procedure muspat_get_packed_size;
procedure muspat_free;

procedure muspatio_open;

procedure __muspatl_set_flags;

procedure muspatl_init;
procedure muspatl_free;

procedure muspatorder_init;
procedure muspatorder_free;

(*$ifdef DEBUG*)

procedure DEBUG_get_pattern_channel_event_str;
procedure DEBUG_dump_pattern_info;
procedure DEBUG_dump_pattern;

(*$endif*)

implementation

uses
    i86,
    string_,
    stdio,
    debug,
    dos_,
    dynarray;

(*$L muspat.obj*)

procedure muspatchnevent_clear; external;
procedure muspatrowevent_clear; external;

procedure __muspat_set_flags; external;

procedure muspat_init; external;
procedure muspat_get_data; external;
procedure muspat_map_EM_data; external;
procedure muspat_set_packed_row_start; external;
procedure muspat_get_packed_row_start; external;
procedure muspat_get_packed_size; external;
procedure muspat_free; external;

procedure muspatio_open; external;

procedure __muspatl_set_flags; external;

procedure muspatl_init; external;
procedure muspatl_free; external;

procedure muspatorder_init; external;
procedure muspatorder_free; external;

(*$ifdef DEBUG*)

procedure DEBUG_get_pattern_channel_event_str; external;
procedure DEBUG_dump_pattern_info; external;
procedure DEBUG_dump_pattern; external;

(*$endif*)

end.
