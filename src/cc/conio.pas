(* conio.pas -- Pascal declarations for custom "conio" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit conio;

interface

(*$I defines.pas*)

(*
procedure cc_inp;
procedure cc_outp;
*)
procedure cc_kbhit;
procedure cc_getch;

procedure cc_clreol;
procedure cc_clrscr;
procedure pascal_gotoxy(x, y: Byte);
procedure pascal_textbackground(color: Byte);
procedure pascal_textcolor(color: Byte);
procedure pascal_textmode(mode: Word);
procedure pascal_window(x1, y1, x2, y2: Byte);

implementation

uses
    i86,
    vbios,
    crt;

(*$L conio\inp.obj*)
(*
procedure cc_inp; external;
*)

(*$L conio\outp.obj*)
(*
procedure cc_outp; external;
*)

(*$L conio\kbhit.obj*)
procedure cc_kbhit; external;

(*$L conio\getch.obj*)
procedure cc_getch; external;

(*$L conio\clreol.obj*)
procedure cc_clreol; external;

(*$L conio\clrscr.obj*)
procedure cc_clrscr; external;

(* CRT Unit *)

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
