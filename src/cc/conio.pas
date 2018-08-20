(* conio.pas -- Pascal declarations for custom "conio" library.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit conio;

interface

uses
    crt;

(*$I defines.pas*)

(*$ifdef DEFINE_LOCAL_DATA*)
var
    (* private, hidden in Crt unit *)
    cc_gotbreak: Boolean;
    cc_lastkey: Char;
    cc_screenwidth: Word;
    cc_screenheight: Word;
    cc_textattrorig: Byte;
    (* publics *)
    cc_checkbreak: Boolean absolute crt.CheckBreak;
    cc_checkeof: Boolean absolute crt.CheckEOF;
    cc_checksnow: Boolean absolute crt.CheckSnow;
    cc_directvideo: Boolean absolute crt.DirectVideo;
    cc_lastmode: Word absolute crt.LastMode;
    cc_textattr: Byte absolute crt.TextAttr;
    cc_windmin: Word absolute crt.WindMin;
    cc_windmax: Word absolute crt.WindMax;
(*$endif*)

(*
procedure cc_inp;
procedure cc_outp;
*)

procedure cc_textmode;
procedure cc_window;
procedure cc_clrscr;
procedure cc_clreol;
procedure cc_gotoxy;
procedure pascal_textbackground(color: Byte);
procedure pascal_textcolor(color: Byte);

procedure cc_kbhit;
procedure cc_getch;

procedure _cc_console_set_mode;
procedure _cc_console_on_mode_change;
procedure _cc_console_on_start;
procedure cc_console_init;

implementation

uses
    i86,
    vbios;

(*$L conio\inp.obj*)
(*
procedure cc_inp; external;
*)

(*$L conio\outp.obj*)
(*
procedure cc_outp; external;
*)

(*$L conio\textmode.obj*)
procedure cc_textmode; external;

(*$L conio\window.obj*)
procedure cc_window; external;

(*$L conio\clrscr.obj*)
procedure cc_clrscr; external;

(*$L conio\clreol.obj*)
procedure cc_clreol; external;

(*$L conio\gotoxy.obj*)
procedure cc_gotoxy; external;

(* CRT Unit *)

procedure pascal_textbackground(color: Byte);
begin
    crt.TextBackground(color);
end;

procedure pascal_textcolor(color: Byte);
begin
    crt.TextColor(color);
end;

(*$L conio\kbhit.obj*)
procedure cc_kbhit; external;

(*$L conio\getch.obj*)
procedure cc_getch; external;

(*$L conio\init.obj*)
procedure _cc_console_set_mode; external;
procedure _cc_console_on_mode_change; external;
procedure _cc_console_on_start; external;
procedure cc_console_init; external;

end.
