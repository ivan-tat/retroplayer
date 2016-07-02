(* Unit Syswrap -- Pascal System unit wrapper.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain. *)

unit syswrap;

interface

function mavail: longint;
function malloc( size: word ): pointer;
procedure memfree( p: pointer; size: word );

implementation

function mavail: longint;
begin
    mavail := System.MaxAvail;
end;

function malloc( size: word ): pointer;
var
    p: pointer;
begin
    GetMem( p, size );
    malloc := p;
end;

procedure memfree( p: pointer; size: word );
begin
  FreeMem( p, size );
end;

end.
