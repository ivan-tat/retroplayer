(* Common defines *)

(*$ifndef _DEFINES_PAS_INCLUDED*)
(*$define _DEFINES_PAS_INCLUDED*)

(*$define DEBUG*)
(*-$define DEBUG_WRITE_LOG*)
(*$define DEBUG_FILE_S3M_LOAD*)
(*-$define DEBUG_WRITE_OUTPUT_STREAM*)
(*$define DEFINE_LOCAL_DATA*)
(*-$define USE_INTRINSICS*)

(*$define CONFIG_VBIOS_ENABLE_LIGHT_PEN*)

(*$else*)   (* _DEFINES_PAS_INCLUDED *)

const
(*$ifdef DEBUG*)
    DEBUG = true;
(*$else*)
    DEBUG = false;
(*$endif*)
(*$ifdef DEBUG_WRITE_LOG*)
    DEBUG_WRITE_LOG = true;
(*$else*)
    DEBUG_WRITE_LOG = false;
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
(*$ifdef DEFINE_LOCAL_DATA*)
    DEFINE_LOCAL_DATA = true;
(*$else*)
    DEFINE_LOCAL_DATA = false;
(*$endif*)
(*$ifdef USE_INTRINSICS*)
    USE_INTRINSICS = true;
(*$else*)
    USE_INTRINSICS = false;
(*$endif*)

(*$ifdef CONFIG_VBIOS_ENABLE_LIGHT_PEN*)
    CONFIG_VBIOS_ENABLE_LIGHT_PEN = true;
(*$else*)
    CONFIG_VBIOS_ENABLE_LIGHT_PEN = false;
(*$endif*);

(*$endif*)  (* _DEFINES_PAS_INCLUDED *)
