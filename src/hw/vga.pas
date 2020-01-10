(* vga.pas -- declarations for vga.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit vga;

interface

(*$I defines.pas*)

procedure vga_wait_sync;
procedure vga_wait_vsync;
procedure vga_set_text_cursor_position;
procedure vga_clear_page_320x200x8;
procedure vga_line;
procedure vga_bar;

procedure register_vga;

implementation

uses
    pascal,
    i86,
    stdlib,
    string_,
    conio,
    debug;

(*$l vga.obj*)

procedure vga_wait_sync; external;
procedure vga_wait_vsync; external;
procedure vga_set_text_cursor_position; external;
procedure vga_clear_page_320x200x8; external;

(*$L vga\bar.obj*)
procedure vga_bar; external;

(*$L vga\line.obj*)
procedure vga_line; external;

procedure register_vga; external;

end.
