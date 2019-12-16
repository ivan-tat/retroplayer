(* strutils.pas -- string utilities.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit
    strutils;

interface

(*$I defines.pas*)

type
    TString2 = string[2];
    TString3 = string[3];
    TString4 = string[4];
    TString8 = string[8];

function hexb(value: byte): TString2;
function hexw(value: word): TString4;
function hexd(value: longint): TString8;
function decb(value: byte): TString3;
function leftformat(s: string; len: byte): string;
function upstr(s: String): String;

implementation

uses
    hexdigts;

function hexb(value: byte): TString2;
begin
    hexb[0] := #2;
    hexb[1] := HEXDIGITS[value shr 4];
    hexb[2] := HEXDIGITS[value and $0f];
end;

function hexw(value: word): TString4;
begin
    hexw[0] := #4;
    hexw[1] := HEXDIGITS[hi(value) shr 4];
    hexw[2] := HEXDIGITS[hi(value) and $0f];
    hexw[3] := HEXDIGITS[lo(value) shr 4];
    hexw[4] := HEXDIGITS[lo(value) and $0f];
end;

function hexd(value: longint): TString8;
var
    w: word;
begin
    hexd[0] := #8;
    w := value shr 16;
    hexd[1] := HEXDIGITS[hi(w) shr 4];
    hexd[2] := HEXDIGITS[hi(w) and $0f];
    hexd[3] := HEXDIGITS[lo(w) shr 4];
    hexd[4] := HEXDIGITS[lo(w) and $0f];
    w := value and $ffff;
    hexd[5] := HEXDIGITS[hi(w) shr 4];
    hexd[6] := HEXDIGITS[hi(w) and $0f];
    hexd[7] := HEXDIGITS[lo(w) shr 4];
    hexd[8] := HEXDIGITS[lo(w) and $0f];
end;


function decb(value: byte): TString3;
var
    s: TString3;
begin
    str(value:3, s);
    decb := s;
end;

function leftformat(s: string; len: byte): string;
var
    t: string;
    i: byte;
begin
    t := s;
    for i := length(s) to len do t := t + ' ';
    t := copy(t, 1, len);
    leftformat := t;
end;

function upstr(s: String): String;
var
    t: String;
    i: Byte;
begin
    t[0] := Chr(length(s));
    for i := 1 to length(s) do
        t[i] := UpCase(s[i]);
    upstr := t;
end;

end.
