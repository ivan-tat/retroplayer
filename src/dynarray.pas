(* dynarray.pas -- Pascal declarations for "dynarray" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    dynarray;

interface

(*$I defines.pas*)

procedure _dynarr_new_;
procedure _dynarr_init_;
procedure _dynarr_clear_list_;
procedure _dynarr_delete_;
procedure _dynarr_set_item_;
procedure _dynarr_get_item_;
procedure _dynarr_set_size_;
procedure _dynarr_get_size_;
procedure _dynarr_free_;

implementation

uses
    pascal,
    dos_,
    string_;

(*$L dynarray.obj*)

procedure _dynarr_new_; external;
procedure _dynarr_init_; external;
procedure _dynarr_clear_list_; external;
procedure _dynarr_delete_; external;
procedure _dynarr_set_item_; external;
procedure _dynarr_get_item_; external;
procedure _dynarr_set_size_; external;
procedure _dynarr_get_size_; external;
procedure _dynarr_free_; external;

end.
