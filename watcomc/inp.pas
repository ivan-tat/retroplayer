(* Declarations for inp.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit inp;

interface

procedure inp_;

implementation

(*$l inp.obj*)

procedure inp_; external;

end.