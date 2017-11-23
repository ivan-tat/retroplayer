(* screen.pas -- Pascal declarations for screen.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit screen;

interface

(*$I defines.pas*)

uses
    s3mtypes,
    mixchn;

const
    scrWidth = 80;
    scrHeight = 25;

type
    TSCRRECT = packed record
        x0, y0, x1, y1: Byte;
    end;
    PSCRRECT = ^TSCRRECT;

type
    TWINFLAGS = Word;

const
    WINFL_VISIBLE     = (1 shl 0);
    WINFL_FOCUSED     = (1 shl 1);
    WINFL_FULLREDRAW  = (1 shl 2);
    WINFL_FOCUSREDRAW = (1 shl 3);
    WINFL_REDRAW      = (WINFL_FULLREDRAW or WINFL_FOCUSREDRAW);

type
    TSCRWIN = packed record
        flags: TWINFLAGS;
        rect: TSCRRECT;
        draw: Pointer;
        keypress: Pointer;
    end;
    PSCRWIN = ^TSCRWIN;

procedure scrwin_init(self: PSCRWIN; x0, y0, x1, y1: Byte; draw, keypress: Pointer);
function  scrwin_is_created(self: PSCRWIN): Boolean;
procedure scrwin_set_flags(self: PSCRWIN; value: TWINFLAGS);
function  scrwin_get_flags(self: PSCRWIN): TWINFLAGS;
procedure scrwin_set_width(self: PSCRWIN; value: Byte);
function  scrwin_get_width(self: PSCRWIN): Byte;
procedure scrwin_set_height(self: PSCRWIN; value: Byte);
function  scrwin_get_height(self: PSCRWIN): Byte;
procedure scrwin_draw(self: PSCRWIN);
function  scrwin_keypress(self: PSCRWIN; c: Char): Boolean;
procedure scrwin_show(self: PSCRWIN);
procedure scrwin_focus(self: PSCRWIN);
procedure scrwin_leave(self: PSCRWIN);
procedure scrwin_close(self: PSCRWIN);

implementation

uses
    conio;

(*$l screen.obj*)

procedure scrwin_init(self: PSCRWIN; x0, y0, x1, y1: Byte; draw, keypress: Pointer); external;
function  scrwin_is_created(self: PSCRWIN): Boolean; external;
procedure scrwin_set_flags(self: PSCRWIN; value: TWINFLAGS); external;
function  scrwin_get_flags(self: PSCRWIN): TWINFLAGS; external;
procedure scrwin_set_width(self: PSCRWIN; value: Byte); external;
function  scrwin_get_width(self: PSCRWIN): Byte; external;
procedure scrwin_set_height(self: PSCRWIN; value: Byte); external;
function  scrwin_get_height(self: PSCRWIN): Byte; external;
procedure scrwin_draw(self: PSCRWIN); external;
function  scrwin_keypress(self: PSCRWIN; c: Char): Boolean; external;
procedure scrwin_show(self: PSCRWIN); external;
procedure scrwin_focus(self: PSCRWIN); external;
procedure scrwin_leave(self: PSCRWIN); external;
procedure scrwin_close(self: PSCRWIN); external;

end.
