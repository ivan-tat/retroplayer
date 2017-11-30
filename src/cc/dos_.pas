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

procedure pascal_swapvectors;
procedure pascal_exec(Name: PathStr; CmdLine: String);

procedure __cc_set_errno_dos;

procedure __cc_doserror;

procedure _cc_dos_getdate;
procedure _cc_dos_gettime;

procedure _cc_dos_getvect;
procedure _cc_dos_setvect;

(*
procedure _cc_dos_para;
procedure _cc_dos_allocmem;
procedure _cc_dos_freemem;
procedure _cc_dos_setblock;
*)
function _dos_para(size: Word): Word;
function _dos_allocmem(size: Word; var seg: Word): Word;
function _dos_freemem(seg: Word): Word;
function _dos_setblock(size: Word; seg: Word; var max: Word): Word;

implementation

uses
    pascal,
    errno_,
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

(*$L dos\gtime086.obj*)
procedure _cc_dos_getdate; external;
procedure _cc_dos_gettime; external;

(*$L dos\d_getvec.obj*)
procedure _cc_dos_getvect; external;

(*$L dos\d_setvec.obj*)
procedure _cc_dos_setvect; external;

(*$L dos\mem086.obj*)
(*
procedure _cc_dos_para; external;
procedure _cc_dos_allocmem; external;
procedure _cc_dos_freemem; external;
procedure _cc_dos_setblock; external;
*)
function _dos_para(size: Word): Word; external;
function _dos_allocmem(size: Word; var seg: Word): Word; external;
function _dos_freemem(seg: Word): Word; external;
function _dos_setblock(size: Word; seg: Word; var max: Word): Word; external;

end.
