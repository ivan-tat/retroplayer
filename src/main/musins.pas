(* musins.pas -- Pascal declarations for musins.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit musins;

interface

(*$I defines.pas*)

procedure musins_init;
procedure musins_set_title;
procedure musins_free;

procedure musinsl_init;
procedure musinsl_free;

(*$ifdef DEBUG*)

procedure DEBUG_dump_instrument_info;

(*$endif*)

implementation

uses
    i86,
    string_,
    stdio,
    debug,
    dos_,
    ems,
    dynarray,
    pcmsmp;

(*$L musins.obj*)

procedure musins_init; external;
procedure musins_set_title; external;
procedure musins_free; external;

procedure musinsl_init; external;
procedure musinsl_free; external;

(*$ifdef DEBUG*)

procedure DEBUG_dump_instrument_info; external;

(*$endif*)

end.
