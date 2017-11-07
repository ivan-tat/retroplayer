(* dynarray.pas -- Pascal declarations for "dynarray" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    dynarray;

interface

(*$I defines.pas*)

procedure dynarr_init_;
procedure dynarr_init_items_;
procedure dynarr_free_items_;
procedure dynarr_set_item_;
procedure dynarr_get_item_;
procedure dynarr_set_size_;
procedure dynarr_get_size_;
procedure dynarr_free_;

implementation

uses
    pascal,
    dos_,
    string_;

(*$L dynarray.obj*)

procedure dynarr_init_; external;
procedure dynarr_init_items_; external;
procedure dynarr_free_items_; external;
procedure dynarr_set_item_; external;
procedure dynarr_get_item_; external;
procedure dynarr_set_size_; external;
procedure dynarr_get_size_; external;
procedure dynarr_free_; external;

end.
