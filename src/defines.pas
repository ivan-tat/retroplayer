(* Common defines *)

(*$ifndef _DEFINES_PAS_INCLUDED*)
(*$define _DEFINES_PAS_INCLUDED*)

(*$define DEBUG*)
(*$define DEBUG_FILE_S3M_LOAD*)
(*-$define DEBUG_WRITE_OUTPUT_STREAM*)

(*$else*)   (* _DEFINES_PAS_INCLUDED *)

const
(*$ifdef DEBUG*)
    DEBUG = true;
(*$else*)
    DEBUG = false;
(*$endif*)
(*$ifdef DEBUG_FILE_S3M_LOAD*)
    DEBUG_FILE_S3M_LOAD = true;
(*$else*)
    DEBUG_FILE_S3M_LOAD = false;
(*$endif*)
(*$ifdef DEBUG_WRITE_OUTPUT_STREAM*)
    DEBUG_WRITE_OUTPUT_STREAM = true;
(*$else*)
    DEBUG_WRITE_OUTPUT_STREAM = false;
(*$endif*)

(*$endif*)  (* _DEFINES_PAS_INCLUDED *)
