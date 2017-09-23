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

procedure pascal_getintvec(num: byte; var p: pointer);
begin
    dos.GetIntVec(num, p);
end;

procedure pascal_setintvec(num: byte; p: pointer);
begin
    dos.SetIntVec(num, p);
end;

end.
