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

(* Text window *)

const
    scrWidth = 80;
    scrHeight = 25;

type
    TWINDOWRECT = packed record
        visible: Boolean;
        focused: Boolean;
        fgColor, bgColor: Byte;
        x0, y0, x1, y1: Byte;
        width, height: Byte;
    end;
    PWINDOWRECT = ^TWINDOWRECT;

procedure window_init(self: PWINDOWRECT; fgColor, bgColor: Byte; x0, y0, x1, y1: Byte);
function window_is_created(self: PWINDOWRECT): Boolean;
procedure window_show(self: PWINDOWRECT);
procedure window_focus(self: PWINDOWRECT);
procedure window_leave(self: PWINDOWRECT);
procedure window_close(self: PWINDOWRECT);

implementation

uses
    conio;

(*$l screen.obj*)

procedure window_init(self: PWINDOWRECT; fgColor, bgColor: Byte; x0, y0, x1, y1: Byte); external;
function window_is_created(self: PWINDOWRECT): Boolean; external;
procedure window_show(self: PWINDOWRECT); external;
procedure window_focus(self: PWINDOWRECT); external;
procedure window_leave(self: PWINDOWRECT); external;
procedure window_close(self: PWINDOWRECT); external;

end.
