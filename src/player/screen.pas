(* screen.pas -- Pascal declarations for screen.c.

   This file is for linking compiled object files with Pascal linker.
   It will be deleted in future when we rewrite the project in C.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. *)

unit screen;

interface

(*$I defines.pas*)

uses
    musdefs,
    mixchn;

procedure scrwin_init;
procedure scrwin_set_rect;
procedure scrwin_set_coords;
procedure scrwin_set_width;
procedure scrwin_get_width;
procedure scrwin_set_height;
procedure scrwin_get_height;
procedure __scrwin_on_resize;
procedure __scrwin_draw;
procedure scrwin_draw;
procedure __scrwin_keypress;
procedure scrwin_keypress;
procedure scrwin_show;
procedure scrwin_focus;
procedure scrwin_leave;
procedure scrwin_close;
procedure __scrwin_free;

implementation

uses
    conio,
    string_,
    common;

(*$l screen.obj*)

procedure scrwin_init; external;
procedure scrwin_set_rect; external;
procedure scrwin_set_coords; external;
procedure scrwin_set_width; external;
procedure scrwin_get_width; external;
procedure scrwin_set_height; external;
procedure scrwin_get_height; external;
procedure __scrwin_on_resize; external;
procedure __scrwin_draw; external;
procedure scrwin_draw; external;
procedure __scrwin_keypress; external;
procedure scrwin_keypress; external;
procedure scrwin_show; external;
procedure scrwin_focus; external;
procedure scrwin_leave; external;
procedure scrwin_close; external;
procedure __scrwin_free; external;

end.
