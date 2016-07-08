(* Declarations for i4d.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit i4d;

interface

procedure __i4d;
procedure __u4d;

implementation

(*$l i4d.obj*)

procedure __i4d; external;
procedure __u4d; external;

end.
