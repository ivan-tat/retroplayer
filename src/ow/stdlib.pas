(* stdlib.pas -- Pascal declarations for stdlib.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit stdlib;

interface

(*$I defines.pas*)

procedure malloc_;
procedure free_;

implementation

uses
    pascal;

(*$l stdlib.obj*)

procedure malloc_; external;
procedure free_; external;

end.
