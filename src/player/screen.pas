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

procedure scrwin_init;
procedure scrwin_is_created;
procedure scrwin_set_flags;
procedure scrwin_get_flags;
procedure scrwin_set_width;
procedure scrwin_get_width;
procedure scrwin_set_height;
procedure scrwin_get_height;
procedure scrwin_draw;
procedure scrwin_keypress;
procedure scrwin_show;
procedure scrwin_focus;
procedure scrwin_leave;
procedure scrwin_close;

implementation

uses
    conio;

(*$l screen.obj*)

procedure scrwin_init; external;
procedure scrwin_is_created; external;
procedure scrwin_set_flags; external;
procedure scrwin_get_flags; external;
procedure scrwin_set_width; external;
procedure scrwin_get_width; external;
procedure scrwin_set_height; external;
procedure scrwin_get_height; external;
procedure scrwin_draw; external;
procedure scrwin_keypress; external;
procedure scrwin_show; external;
procedure scrwin_focus; external;
procedure scrwin_leave; external;
procedure scrwin_close; external;

end.
