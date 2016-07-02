(* Declarations for dointr.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit dointr;

interface

procedure _DoINTR_;

implementation

(*$l dointr.obj*)

procedure _DoINTR_; external;

end.
