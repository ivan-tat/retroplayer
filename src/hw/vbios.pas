(* vbios.pas -- declarations for vbios.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit vbios;

interface

(*$I defines.pas*)

procedure vbios_set_mode;
procedure vbios_set_cursor_shape;

implementation

uses
    pascal,
    i86,
    string_;

(*$l vbios.obj*)

procedure vbios_set_mode; external;
procedure vbios_set_cursor_shape; external;

end.
