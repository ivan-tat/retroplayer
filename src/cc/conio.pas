(* conio.pas -- Pascal declarations for custom "conio" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit conio;

interface

(*$I defines.pas*)

(*
procedure _cc_inp;
procedure _cc_outp;
*)

function  pascal_keypressed: Boolean;
function  pascal_readkey: Char;
procedure pascal_clreol;
procedure pascal_clrscr;
procedure pascal_gotoxy(x, y: Byte);
procedure pascal_textbackground(color: Byte);
procedure pascal_textcolor(color: Byte);
procedure pascal_textmode(mode: Word);
procedure pascal_window(x1, y1, x2, y2: Byte);

implementation

uses
    crt;

(*$L conio\inp.obj*)
(*
procedure _cc_inp; external;
*)

(*$L conio\outp.obj*)
(*
procedure _cc_outp; external;
*)

(* CRT Unit *)

function pascal_keypressed: Boolean;
begin
    pascal_keypressed := crt.KeyPressed;
end;

function pascal_readkey: Char;
begin
    pascal_readkey := crt.ReadKey;
end;

procedure pascal_clreol;
begin
    crt.ClrEol;
end;

procedure pascal_clrscr;
begin
    crt.ClrScr;
end;

procedure pascal_gotoxy(x, y: Byte);
begin
    crt.GotoXY(x, y);
end;

procedure pascal_textbackground(color: Byte);
begin
    crt.TextBackground(color);
end;

procedure pascal_textcolor(color: Byte);
begin
    crt.TextColor(color);
end;

procedure pascal_textmode(mode: Word);
begin
    crt.TextMode(mode);
end;

procedure pascal_window(x1, y1, x2, y2: Byte);
begin
    crt.Window(x1, y1, x2, y2);
end;

end.
