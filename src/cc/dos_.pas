(* dos_.pas -- Pascal declarations for custom "dos" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    dos_;

interface

uses
    dos;

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)

var
    _cc_doserrno: Integer;
const
    SAVEINTVEC_COUNT = 19;
    SaveIntVecIndexes: array [0..SAVEINTVEC_COUNT-1] of Byte =
    (
        $00, $02, $1B, $21, $23, $24, $34, $35, $36, $37,
        $38, $39, $3A, $3B, $3C, $3D, $3E, $3F, $75
    );
var
    SaveIntVecs: array [0..SAVEINTVEC_COUNT-1] of Pointer;

(*$endif*)  (* DEFINE_LOCAL_DATA *)

procedure pascal_swapvectors;
procedure pascal_exec(Name: PathStr; CmdLine: String);

procedure __cc_set_errno_dos;

procedure __cc_doserror;
procedure __cc_doserror2;

procedure cc_dosexterr;

procedure _cc_local_int24_asm;

procedure _cc_dos_console_out;
procedure _cc_dos_console_in;

procedure _cc_dos_getdate;
procedure _cc_dos_gettime;

procedure _cc_dos_getvect;
procedure _cc_dos_setvect;

procedure _cc_dos_para;
procedure _cc_dos_allocmem;
procedure _cc_dos_freemem;
procedure _cc_dos_setblock;

procedure _cc_dos_getpsp;
procedure _cc_dos_getmasterpsp;

procedure _cc_dos_creat;
procedure _cc_dos_creatnew;

procedure _cc_dos_open;

procedure _cc_dos_close;
procedure _cc_dos_commit;

procedure _cc_dos_read;
procedure _cc_dos_write;

procedure _cc_dos_seek;

procedure _cc_dos_ioctl_query_flags;

procedure _cc_dos_parsfnm;
procedure __cc_dos_exec_asm;
procedure _cc_dos_exec;

procedure cc_dos_savevectors;
procedure cc_dos_restorevectors;
procedure cc_dos_swapvectors;
procedure _cc_dos_terminate;

implementation

uses
    pascal,
    errno_,
    string_,
    i86;

procedure pascal_swapvectors;
begin
    Dos.SwapVectors;
end;

procedure pascal_exec(Name: PathStr; CmdLine: String);
begin
    Dos.Exec(Name, CmdLine);
end;

(*$L dos\dosret.obj*)
procedure __cc_set_errno_dos; external;

(*$L dos\error086.obj*)
procedure __cc_doserror; external;
procedure __cc_doserror2; external;

(*$L dos\dosexter.obj*)
procedure cc_dosexterr; external;

(*$L dos\int24.obj*)
procedure _cc_local_int24_asm; external;

(*$L dos\coniodos.obj*)
procedure _cc_dos_console_out; external;
procedure _cc_dos_console_in; external;

(*$L dos\gtime086.obj*)
procedure _cc_dos_getdate; external;
procedure _cc_dos_gettime; external;

(*$L dos\d_getvec.obj*)
procedure _cc_dos_getvect; external;

(*$L dos\d_setvec.obj*)
procedure _cc_dos_setvect; external;

(*$L dos\mem086.obj*)
procedure _cc_dos_para; external;
procedure _cc_dos_allocmem; external;
procedure _cc_dos_freemem; external;
procedure _cc_dos_setblock; external;

(*$L dos\psp.obj*)
procedure _cc_dos_getpsp; external;
procedure _cc_dos_getmasterpsp; external;

(*$L dos\creatdos.obj*)
procedure _cc_dos_creat; external;
procedure _cc_dos_creatnew; external;

(*$L dos\opendos.obj*)
procedure _cc_dos_open; external;

(*$L dos\filedos.obj*)
procedure _cc_dos_close; external;
procedure _cc_dos_commit; external;

(*$L dos\io086.obj*)
procedure _cc_dos_read; external;
procedure _cc_dos_write; external;

(*$L dos\seekdos.obj*)
procedure _cc_dos_seek; external;

(*$L dos\dosioctl.obj*)
procedure _cc_dos_ioctl_query_flags; external;

(*$L dos\parsfnm.obj*)
procedure _cc_dos_parsfnm; external;

(*$L dos\ex.obj*)
procedure __cc_dos_exec_asm; external;

(*$L dos\exec.obj*)
procedure _cc_dos_exec; external;

(*$L dos\c_init.obj*)
procedure cc_dos_savevectors; external;
procedure cc_dos_restorevectors; external;

(*$L dos\c_swap.obj*)
procedure cc_dos_swapvectors; external;

(*$L dos\dosterm.obj*)
procedure _cc_dos_terminate; external;

end.
