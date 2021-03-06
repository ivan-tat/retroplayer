(* filldma.pas -- Pascal declarations for filldma.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit filldma;

interface

uses
    mixer,
    fillvars;

(*$I defines.pas*)

(*$ifdef DEBUG_WRITE_OUTPUT_STREAM*)

(*$ifdef DEFINE_LOCAL_DATA*)

var
    _debug_stream: array [0..1] of Pointer;

(*$endif*)  (* DEFINE_LOCAL_DATA *)

procedure DEBUG_open_output_streams;
procedure DEBUG_close_output_streams;

(*$endif*)  (*DEBUG_WRITE_OUTPUT_STREAM*)

procedure fill_DMAbuffer;

implementation

uses
    string_,
(*$ifdef DEBUG*)
    stdio,
(*$endif*)  (* DEBUG *)
(*$ifdef DEBUG_WRITE_OUTPUT_STREAM*)
    dos_,
(*$endif*)  (*DEBUG_WRITE_OUTPUT_STREAM*)
    sndctl_t,
    musmodps,
    mixing,
    posttab;

(*$l filldma.obj*)

(*$ifdef DEBUG_WRITE_OUTPUT_STREAM*)

procedure DEBUG_open_output_streams; external;
procedure DEBUG_close_output_streams; external;

(*$endif*)  (*DEBUG_WRITE_OUTPUT_STREAM*)

procedure fill_DMAbuffer; external;

end.
