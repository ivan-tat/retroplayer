(* string_.pas -- Pascal declarations for custom "string" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit string_;

interface

(*$I defines.pas*)

procedure cc_memcmp;
procedure cc_memcpy;
procedure cc_memset;
procedure cc_strchr;
procedure cc_strcmp;
procedure cc_stricmp;
procedure cc_strlen;
procedure cc_strncmp;
procedure cc_strncpy;
procedure cc_strnicmp;

function  memcmp(var buf1, buf2; size: Word): Integer;
procedure memcpy(var dest, src; size: Word);
procedure memset(var dest; value: Byte; size: Word);
function  strlen(src: PChar): Word;
function  strncmp(str1, str2: PChar; maxlen: Word): Integer;
function  strncpy(dest, src: PChar; maxlen: Word): PChar;

implementation

(*$L string/memcmp.obj*)
procedure cc_memcmp; external;

(*$L string/memcpy.obj*)
procedure cc_memcpy; external;

(*$L string/memset.obj*)
procedure cc_memset; external;

(*$L string/strchr.obj*)
procedure cc_strchr; external;

(*$L string/strcmp.obj*)
procedure cc_strcmp; external;

(*$L string/stricmp.obj*)
procedure cc_stricmp; external;

(*$L string/strlen.obj*)
procedure cc_strlen; external;

(*$L string/strncmp.obj*)
procedure cc_strncmp; external;

(*$L string/strncpy.obj*)
procedure cc_strncpy; external;

(*$L string/strnicmp.obj*)
procedure cc_strnicmp; external;

function memcmp(var buf1, buf2; size: Word): Integer;
var
    s1, s2: PChar;
    count: Word;
    cmp: Integer;
begin
    s1 := @buf1;
    s2 := @buf2;
    cmp := 0;
    count := size;
    while (count > 0) do
    begin
        cmp := ord(s1[0]) - ord(s2[0]);
        if (cmp <> 0) then
        begin
            memcmp := cmp;
            exit;
        end;
        dec(count);
    end;
    memcmp := cmp;
end;

procedure memcpy(var dest, src; size: Word);
begin
    move(src, dest, size);
end;

procedure memset(var dest; value: Byte; size: Word);
begin
    fillchar(dest, size, value);
end;

function strlen(src: PChar): Word;
var
    cur: PChar;
begin
    cur := src;
    while (cur[0] <> #0) do
        inc(cur);
    strlen := cur - src;
end;

function strncmp(str1, str2: PChar; maxlen: Word): Integer;
var
    s1, s2: PChar;
    count: Word;
    cmp: Integer;
begin
    s1 := @str1;
    s2 := @str2;
    cmp := 0;
    count := maxlen;
    while (count > 0) do
    begin
        cmp := ord(s1[0]) - ord(s2[0]);
        if ((cmp <> 0) or (ord(s1[0]) = 0) or (ord(s2[0]) = 0)) then
        begin
            strncmp := cmp;
            exit;
        end;
        inc(s1);
        inc(s2);
        dec(count);
    end;
    strncmp := cmp;
end;

function strncpy(dest, src: PChar; maxlen: Word): PChar;
var
    d, s: PChar;
    count: Word;
begin
    d := dest;
    s := src;
    count := maxlen;
    while ((count > 0) and (s[0] <> #0)) do
    begin
        d[0] := s[0];
        inc(s);
        inc(d);
        dec(count);
    end;
    if (count > 0) then
        memset(d^, 0, count);
    strncpy := dest;
end;

end.
