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

const
    hex: array [0..15 ] of char = '0123456789ABCDEF';

function hexb(value: byte): TString2;
begin
    hexb[0] := #2;
    hexb[1] := hex[value shr 4];
    hexb[2] := hex[value and $0f];
end;

function hexw(value: word): TString4;
begin
    hexw[0] := #4;
    hexw[1] := hex[hi(value) shr 4];
    hexw[2] := hex[hi(value) and $0f];
    hexw[3] := hex[lo(value) shr 4];
    hexw[4] := hex[lo(value) and $0f];
end;

function hexd(value: longint): TString8;
var
    w: word;
begin
    hexd[0] := #8;
    w := value shr 16;
    hexd[1] := hex[hi(w) shr 4];
    hexd[2] := hex[hi(w) and $0f];
    hexd[3] := hex[lo(w) shr 4];
    hexd[4] := hex[lo(w) and $0f];
    w := value and $ffff;
    hexd[5] := hex[hi(w) shr 4];
    hexd[6] := hex[hi(w) and $0f];
    hexd[7] := hex[lo(w) shr 4];
    hexd[8] := hex[lo(w) and $0f];
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
