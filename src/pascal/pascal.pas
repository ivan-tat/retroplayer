(* pascal.pas -- support for Pascal linker and Pascal units wrapper.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit pascal;

interface

(*$I defines.pas*)

(* System Unit *)

procedure pascal_halt(exitcode: word);

function  pascal_maxavail: longint;
procedure pascal_getmem(var p: pointer; size: word);
procedure pascal_freemem(p: pointer; size: word);

procedure pascal_assign(var f: file; path: PChar);
function  pascal_reset(var f: file): boolean;
function  pascal_rewrite(var f: file): boolean;
procedure pascal_close(var f: file);
function  pascal_seek(var f: file; pos: longint): boolean;
function  pascal_blockread(var f: file; var buf; size: word; var actual: word): boolean;
function  pascal_blockwrite(var f: file; var buf; size: word; var actual: word): boolean;

(* CRT Unit *)

procedure pascal_delay(count: word);

(* DOS Unit *)

procedure pascal_getintvec(num: byte; var p: pointer);
procedure pascal_setintvec(num: byte; p: pointer);

implementation

uses
    crt,
    dos;

procedure pascal_halt(exitcode: word);
begin
    System.Halt(exitcode);
end;

function pascal_maxavail: longint;
begin
    pascal_maxavail := System.MaxAvail;
end;

procedure pascal_getmem(var p: pointer; size: word);
begin
    System.GetMem(p, size);
end;

procedure pascal_freemem(p: pointer; size: word);
begin
    System.FreeMem(p, size);
end;

procedure pascal_assign(var f: file; path: pchar);
begin
    System.Assign(f, path);
end;

function pascal_reset(var f: file): boolean;
begin
    (*$I-*)
    System.Reset(f, 1);
    (*$I+*)
    pascal_reset := IOResult = 0;
end;

function pascal_rewrite(var f: file): boolean;
begin
    (*$I-*)
    System.Rewrite(f, 1);
    (*$I+*)
    pascal_rewrite := IOResult = 0;
end;

procedure pascal_close(var f: file);
begin
    System.Close(f);
end;

function pascal_seek(var f: file; pos: longint): boolean;
begin
    (*$I-*)
    System.Seek(f, pos);
    (*$I+*)
    pascal_seek := IOResult = 0;
end;

function pascal_blockread(var f: file; var buf; size: word; var actual: word): boolean;
begin
    (*$I-*)
    System.BlockRead(f, buf, size, actual);
    (*$I+*)
    pascal_blockread := IOResult = 0;
end;

function pascal_blockwrite(var f: file; var buf; size: word; var actual: word): boolean;
begin
    (*$I-*)
    System.BlockWrite(f, buf, size, actual);
    (*$I+*)
    pascal_blockwrite := IOResult = 0;
end;

procedure pascal_delay(count: word);
begin
    crt.Delay(count);
end;

procedure pascal_getintvec(num: byte; var p: pointer);
begin
    dos.GetIntVec(num, p);
end;

procedure pascal_setintvec(num: byte; p: pointer);
begin
    dos.SetIntVec(num, p);
end;

end.
