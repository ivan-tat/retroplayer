(* conio.pas -- Pascal declarations for conio C library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit conio;

interface

procedure inp_;
procedure outp_;

implementation

(*$l inp.obj*)
procedure inp_; external;

(*$l outp.obj*)
procedure outp_; external;

end.
