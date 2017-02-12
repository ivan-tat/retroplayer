(* i86.pas -- Pascal declarations for i86.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit i86;

interface

procedure delay_;
procedure intr_;
procedure _DoINTR_;

implementation

uses
    pascal;

(*$l i86.obj*)
procedure delay_; external;

(*$l intr.obj*)
procedure intr_; external;

(*$l dointr.obj*)
procedure _DoINTR_; external;

end.
