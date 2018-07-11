(* io.pas -- Pascal declarations for custom "io" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit io;

interface

(*$I defines.pas*)

procedure cc_open;
procedure cc_close;
procedure cc_lseek;
procedure cc_read;
procedure cc_write;

implementation

uses
    watcom,
    pascal,
    debugfn,
    string_,
    ctype,
    errno_,
    dos,
    dos_;

(*$L io/open.obj*)
procedure cc_open; external;

(*$L io/close.obj*)
procedure cc_close; external;

(*$L io/lseek.obj*)
procedure cc_lseek; external;

(*$L io/read.obj*)
procedure cc_read; external;

(*$L io/write.obj*)
procedure cc_write; external;

end.
