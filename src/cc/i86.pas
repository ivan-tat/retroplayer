(* i86.pas -- Pascal declarations for custom "i86" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit i86;

interface

(*$I defines.pas*)

procedure cc_delay_;
procedure cc_intr_;
procedure _cc_DoINTR_;

implementation

uses
    i4m,
    pascal;

(*$L i86\delay.obj*)
procedure cc_delay_; external;

(*$L i86\intr.obj*)
procedure cc_intr_; external;

(*$L i86\dointr.obj*)
procedure _cc_DoINTR_; external;

end.
