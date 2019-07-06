(* pcmsmp.pas -- Pascal declarations for pcmsmp.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit pcmsmp;

interface

(*$I defines.pas*)

procedure __pcmsmp_set_flags;

procedure pcmsmp_init;
procedure pcmsmp_map_EM_data;
procedure pcmsmp_set_title;
procedure pcmsmp_free;

procedure __pcmsmpl_set_flags;

procedure pcmsmpl_init;
procedure pcmsmpl_free;

(*$ifdef DEBUG*)

procedure DEBUG_dump_sample_info;

(*$endif*)  (* DEBUG *)

implementation

uses
    i86,
    string_,
    stdio,
    dos_,
    debug,
    ems,
    dynarray;

(*$l pcmsmp.obj*)

procedure __pcmsmp_set_flags; external;

procedure pcmsmp_init; external;
procedure pcmsmp_map_EM_data; external;
procedure pcmsmp_set_title; external;
procedure pcmsmp_free; external;

procedure __pcmsmpl_set_flags; external;

procedure pcmsmpl_init; external;
procedure pcmsmpl_free; external;

(*$ifdef DEBUG*)

procedure DEBUG_dump_sample_info; external;

(*$endif*)  (* DEBUG *)

end.
