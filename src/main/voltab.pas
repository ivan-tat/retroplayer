(* voltab.pas -- Pascal declarations for voltab.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit voltab;

interface

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)

var
    VolumeTablePtr: Pointer;

(*$endif*)  (* DEFINE_LOCAL_DATA *)

procedure voltab_init;
procedure voltab_alloc;
procedure voltab_calc;
procedure voltab_free;

implementation

uses
    string_,
(*$ifdef DEBUG*)
    stdio,
(*$endif*)  (* DEBUG *)
    dos_;

(*$l voltab.obj*)
procedure voltab_init; external;
procedure voltab_alloc; external;
procedure voltab_calc; external;
procedure voltab_free; external;

end.
