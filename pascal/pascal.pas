(* pascal.pas -- support for Pascal linker and Pascal units wrapper.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit pascal;

interface

function  mavail: longint;
function  malloc(size: word): pointer;
procedure memfree(p: pointer; size: word);

implementation

uses
    crt;

function mavail: longint;
begin
    mavail := System.MaxAvail;
end;

function malloc(size: word): pointer;
var
    p: pointer;
begin
    System.GetMem(p, size);
    malloc := p;
end;

procedure memfree(p: pointer; size: word);
begin
    System.FreeMem(p, size);
end;

end.
