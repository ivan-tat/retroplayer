(* dynarray.pas -- Pascal declarations for "dynarray" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    dynarray;

interface

(*$I defines.pas*)

procedure dynarr_init;
procedure dynarr_init_items;
procedure dynarr_free_items;
procedure dynarr_set_item;
procedure dynarr_get_item;
procedure dynarr_indexof;
procedure dynarr_set_size;
procedure dynarr_get_size;
procedure dynarr_free;

implementation

uses
    pascal,
    dos_,
    string_;

(*$L dynarray.obj*)

procedure dynarr_init; external;
procedure dynarr_init_items; external;
procedure dynarr_free_items; external;
procedure dynarr_set_item; external;
procedure dynarr_get_item; external;
procedure dynarr_indexof; external;
procedure dynarr_set_size; external;
procedure dynarr_get_size; external;
procedure dynarr_free; external;

end.
