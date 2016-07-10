(* Declarations for memset.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit memset;

interface

procedure memset_;

implementation

(*$l memset.obj*)

procedure memset_; external;

end.
