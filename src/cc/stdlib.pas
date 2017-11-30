(* stdlib.pas -- Pascal declarations for custom "stdlib" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit stdlib;

interface

(*$I defines.pas*)

procedure cc_atoi_;
procedure cc_atol_;

procedure cc_strtol_;

procedure cc_malloc_;
procedure cc_free_;

procedure cc_exit_;

procedure custom_getenv_;

implementation

uses
    watcom,
    pascal,
    string_,
    ctype,
    errno_,
    dos;

procedure pascal_halt(exitcode: word); far;
begin
    System.Halt(exitcode);
end;

procedure pascal_getmem(var p: pointer; size: word); far;
begin
    System.GetMem(p, size);
end;

procedure pascal_freemem(p: pointer; size: word); far;
begin
    System.FreeMem(p, size);
end;

(*$L stdlib/atoi.obj*)
procedure cc_atoi_; external;
(*$L stdlib/atol.obj*)
procedure cc_atol_; external;

(*$L stdlib/strtol.obj*)
procedure cc_strtol_; external;

(*$L stdlib/fmalloc.obj*)
procedure cc_malloc_; external;
(*$L stdlib/ffree.obj*)
procedure cc_free_; external;

(*$L stdlib/fexit.obj*)
procedure cc_exit_; external;

function pascal_getenv(name: String): String; near;
begin
    pascal_getenv := getenv(name);
end;

(*$L stdlib/getenv.obj*)
procedure custom_getenv_; external;

end.
