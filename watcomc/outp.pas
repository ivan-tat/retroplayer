(* outp.pas -- Pascal declarations for outp.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit outp;

interface

procedure outp_;

implementation

(*$l outp.obj*)

procedure outp_; external;

end.
