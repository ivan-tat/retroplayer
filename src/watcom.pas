(* watcom.pas -- Pascal declarations for Watcom C internal functions.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    watcom;

interface

(*$I defines.pas*)

procedure __i4d;
procedure __u4d;
procedure __i4m;
procedure __u4m;
procedure __u8dq;
procedure __u8dr;
procedure __i8dq;
procedure __i8dr;
procedure __u8dqe;
procedure __u8dre;
procedure __i8dqe;
procedure __i8dre;

implementation

(*$L i4d.obj*)
procedure __i4d; external;
procedure __u4d; external;

(*$L i4m.obj*)
procedure __i4m; external;
procedure __u4m; external;

(*$L i8d086.obj*)
procedure __u8dq; external;
procedure __u8dr; external;
procedure __i8dq; external;
procedure __i8dr; external;
procedure __u8dqe; external;
procedure __u8dre; external;
procedure __i8dqe; external;
procedure __i8dre; external;

end.
