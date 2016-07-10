(* Declarations for i4m.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit i4m;

interface

procedure __i4m;
procedure __u4m;

implementation

(*$l i4m.obj*)

procedure __i4m; external;
procedure __u4m; external;

end.
