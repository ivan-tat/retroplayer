(* vbios.pas -- declarations for vbios.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit vbios;

interface

(*$I defines.pas*)

procedure vbiosda_get_text_width;
procedure vbiosda_get_text_height;
procedure vbios_set_mode;
procedure vbios_set_cursor_shape;
procedure vbios_set_cursor_pos;
procedure vbios_query_cursor_state;
(*$ifdef CONFIG_VBIOS_ENABLE_LIGHT_PEN*)
procedure vbios_read_light_pen;
(*$endif*)  (* CONFIG_VBIOS_ENABLE_LIGHT_PEN *)
procedure vbios_set_active_page;
procedure vbios_scroll_up;
procedure vbios_scroll_down;

implementation

uses
    pascal,
    i86,
    string_;

(*$l vbios.obj*)

procedure vbiosda_get_text_width; external;
procedure vbiosda_get_text_height; external;
procedure vbios_set_mode; external;
procedure vbios_set_cursor_shape; external;
procedure vbios_set_cursor_pos; external;
procedure vbios_query_cursor_state; external;
(*$ifdef CONFIG_VBIOS_ENABLE_LIGHT_PEN*)
procedure vbios_read_light_pen; external;
(*$endif*)  (* CONFIG_VBIOS_ENABLE_LIGHT_PEN *)
procedure vbios_set_active_page; external;
procedure vbios_scroll_up; external;
procedure vbios_scroll_down; external;

end.
