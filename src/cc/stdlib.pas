(* stdlib.pas -- Pascal declarations for custom "stdlib" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit stdlib;

interface

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)

var
    cc_heap_org: Pointer;
    cc_heap_end: Pointer;
    cc_heap_ptr: Pointer;
    cc_heap_free_list: Pointer;
    cc_heap_error: Pointer;
    cc_environ: Pointer;

(*$endif*)  (* DEFINE_LOCAL_DATA *)

procedure cc_atexit;

procedure cc_atoi;
procedure cc_atol;

procedure cc_strtol;

procedure cc_maxavail;
procedure cc_memavail;
procedure cc_malloc;
procedure cc_free;

procedure cc_exit;

procedure cc_getenv;
procedure cc_unsetenv;
procedure cc_setenv;

procedure cc_heap_init;
procedure environ_init;

implementation

uses
    watcom,
    pascal,
    startup,
    sysdbg,
    string_,
    ctype,
    errno_,
    dos,
    dos_,
    malloc;

(* Internals *)

(*$L stdlib/crwdata.obj*)

(*$ifdef DEFINE_LOCAL_DATA*)

type
    dosenvlist_p = ^dosenvlist_t;
    dosenvlist_t = packed record
        arr: PChar;
        size: Word;
    end;

type
    envstrlist_t = packed record
        arr: Pointer;
        size: Word;
    end;

var
    _dos_env: dosenvlist_t;
    _env_list: envstrlist_t;

(*$endif*)  (* DEFINE_LOCAL_DATA *)

procedure pascal_getmem(var p: pointer; size: word); far;
begin
    System.GetMem(p, size);
end;

procedure pascal_freemem(p: pointer; size: word); far;
begin
    System.FreeMem(p, size);
end;

(* Publics *)

(*$L stdlib/atexit.obj*)
procedure cc_atexit; external;

(*$L stdlib/atoi.obj*)
procedure cc_atoi; external;
(*$L stdlib/atol.obj*)
procedure cc_atol; external;

(*$L stdlib/strtol.obj*)
procedure cc_strtol; external;

(*$L stdlib/havail.obj*)
procedure _cc_heap_avail_func; external;

(*$L stdlib/hmaxavl.obj*)
procedure cc_maxavail; external;

(*$L stdlib/hmemavl.obj*)
procedure cc_memavail; external;

(*$L stdlib/fmalloc.obj*)
procedure cc_malloc; external;

(*$L stdlib/ffree.obj*)
procedure cc_free; external;

(*$L stdlib/exit.obj*)
procedure cc_exit; external;

(*$L stdlib/getenv.obj*)
procedure cc_getenv; external;

(*$L stdlib/unsetenv.obj*)
procedure cc_unsetenv; external;

(*$L stdlib/setenv.obj*)
procedure cc_setenv; external;

(*$L stdlib/heap.obj*)
procedure cc_heap_init; external;

(*$L stdlib/_env.obj*)
procedure environ_init; external;

end.
