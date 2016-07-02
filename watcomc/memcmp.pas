(* Declarations for intr.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit memcmp;

interface

procedure memcmp_;

implementation

(*$l memcmp.obj*)

procedure memcmp_; external;

end.
