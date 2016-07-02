(* Declarations for intr.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit intr;

interface

procedure intr_;

implementation

uses dointr;

(*$l intr.obj*)

procedure intr_; external;

end.
