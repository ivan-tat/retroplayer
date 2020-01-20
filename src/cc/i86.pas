(* i86.pas -- Pascal declarations for custom "i86" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit i86;

interface

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)

var
    _cc_delay_base_ticks: array [0..7] of Byte;

(*$endif*)  (* DEFINE_LOCAL_DATA *)

procedure cc_delay_init;
procedure cc_delay;
procedure cc_intr;
procedure _cc_DoINTR;

implementation

uses
    pascal,
    watcom,
    sysdbg;

(*$L i86\delay.obj*)
procedure cc_delay_init; external;
procedure cc_delay; external;

(*$L i86\intr.obj*)
procedure cc_intr; external;

(*$L i86\dointr.obj*)
procedure _cc_DoINTR; external;

end.
