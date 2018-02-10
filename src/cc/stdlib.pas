(* stdlib.pas -- Pascal declarations for custom "stdlib" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit stdlib;

interface

(*$I defines.pas*)

const
    _cc_psp: Word = 0;

procedure cc_atoi;
procedure cc_atol;

procedure cc_strtol;

procedure cc_malloc;
procedure cc_free;

procedure cc_exit;

procedure custom_getenv;

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
procedure cc_atoi; external;
(*$L stdlib/atol.obj*)
procedure cc_atol; external;

(*$L stdlib/strtol.obj*)
procedure cc_strtol; external;

(*$L stdlib/fmalloc.obj*)
procedure cc_malloc; external;
(*$L stdlib/ffree.obj*)
procedure cc_free; external;

(*$L stdlib/fexit.obj*)
procedure cc_exit; external;

(*$L stdlib/crwdata.obj*)

function pascal_getenv(name: String): String; near;
begin
    pascal_getenv := getenv(name);
end;

(*$L stdlib/getenv.obj*)
procedure custom_getenv; external;

end.
