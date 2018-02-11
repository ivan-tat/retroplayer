(* vga.pas -- declarations for vga.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit vga;

interface

(*$I defines.pas*)

procedure vbios_set_mode(mode: Byte);
procedure vbios_set_cursor_shape(start, stop: Byte);

procedure vga_wait_vsync;

implementation

uses
    pascal,
    i86,
    conio,
    debug;

(*$l vga.obj*)

procedure vbios_set_mode(mode: Byte); external;
procedure vbios_set_cursor_shape(start, stop: Byte); external;

procedure vga_wait_vsync; external;

procedure register_vga; far; external;

begin
    register_vga;
end.
