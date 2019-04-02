(* loads3m.pas -- Pascal declarations for loads3m.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit loads3m;

interface

(*$I defines.pas*)

procedure load_s3m_new;
procedure load_s3m_init;
procedure load_s3m_load;
procedure load_s3m_get_error;
procedure load_s3m_free;
procedure load_s3m_delete;

implementation

uses
    watcom,
    stdio,
    string_,
    dynarray,
    debug,
    strutils,
    crt,
    dos,
    dos_,
    ems,
    common,
    s3mtypes,
    pcmsmp,
    musins,
    muspat,
    musmod,
    mixchn;

(*$l loads3m.obj*)

procedure load_s3m_new; external;
procedure load_s3m_init; external;
procedure load_s3m_load; external;
procedure load_s3m_get_error; external;
procedure load_s3m_free; external;
procedure load_s3m_delete; external;

end.
