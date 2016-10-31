(* Declarations for i8d086.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit i8d086;

interface

procedure __u8dq;
procedure __u8dr;
procedure __i8dq;
procedure __i8dr;
procedure __u8dqe;
procedure __u8dre;
procedure __i8dqe;
procedure __i8dre;

implementation

(*$l i8d086.obj*)

procedure __u8dq; external;
procedure __u8dr; external;
procedure __i8dq; external;
procedure __i8dr; external;
procedure __u8dqe; external;
procedure __u8dre; external;
procedure __i8dqe; external;
procedure __i8dre; external;

end.
