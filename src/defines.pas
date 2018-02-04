(* Common defines *)

(*$ifndef _DEFINES_PAS_INCLUDED*)
(*$define _DEFINES_PAS_INCLUDED*)

(*$define DEBUG*)
(*$define DEBUGLOAD*)
(*-$define DEBUG_WRITE_OUTPUT_STREAM*)

(*$else*)   (* _DEFINES_PAS_INCLUDED *)

const
(*$ifdef DEBUG*)
    DEBUG = true;
(*$else*)
    DEBUG = false;
(*$endif*)
(*$ifdef DEBUGLOAD*)
    DEBUGLOAD = true;
(*$else*)
    DEBUGLOAD = false;
(*$endif*)
(*$ifdef DEBUG_WRITE_OUTPUT_STREAM*)
    DEBUG_WRITE_OUTPUT_STREAM = true;
(*$else*)
    DEBUG_WRITE_OUTPUT_STREAM = false;
(*$endif*)

(*$endif*)  (* _DEFINES_PAS_INCLUDED *)
