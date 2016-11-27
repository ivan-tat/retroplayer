(* readnote.pas -- Pascal declarations for readnote.asm.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit readnote;

interface

procedure set_tempo(tempo: byte);

procedure readnewnotes;
procedure SetupNewInst(dChn: pointer; bInsNum: byte); (* internal use *)
procedure SetNewNote(dChn: pointer; bNote: byte; bKeep: boolean); (* internal use *)

implementation

uses
    emstool,
    s3mtypes,
    s3mvars,
    effvars,
    mixvars,
    mixer,
    mixer_;

procedure set_tempo(tempo: byte);
begin
    if (tempo >= 32) then
        curtempo := tempo
    else
        tempo := curtempo;
    if (tempo <> 0) then BPT := longint(UseRate)*5 div (2*tempo);
end;

(*$l readnote.obj*)
procedure readnewnotes; external;
procedure SetupNewInst(dChn: pointer; bInsNum: byte); external;
procedure SetNewNote(dChn: pointer; bNote: byte; bKeep: boolean); external;

end.
