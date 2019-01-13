/* w_hlp.c -- help window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/conio.h"
#include "cc/stdio.h"
#include "common.h"
#include "player/screen.h"

#include "player/w_hlp.h"

/*
void __far win_help_on_resize (SCRWIN *self);
*/
void __far win_help_draw (SCRWIN *self);
/*
void __far win_help_keypress (SCRWIN *self, char key);
void __far win_help_free (SCRWIN *self);
*/

static const SCRWINVMT __win_help_vmt =
{
    /*
    &win_help_on_resize,
    */
    NULL,
    &win_help_draw,
    /*
    &win_help_keypress,
    &win_help_free
    */
    NULL,
    NULL
};

/* private data */

/* private methods */

/* public methods */

bool __far win_help_init (SCRWIN *self)
{
    scrwin_init (self, "help window");
    _copy_vmt (self, __win_help_vmt, SCRWINVMT);
    return true;
}

/*
void __far win_help_on_resize (SCRWIN *self)
{
}
*/

void __far win_help_draw(SCRWIN *self)
{
    if (scrwin_is_created(self))
    {
        textbackground(_black);
        textcolor(_lightgray);

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            clrscr ();
            textcolor (_white);
            printf (
                "Global keys:" CRLF
            );
            textcolor (_lightgray);
            printf (
                "  F1 .... this help screen" CRLF
                "  F2 .... display channels information" CRLF
                "  F3 .... display current pattern" CRLF
                "  F4 .... display instruments list" CRLF
                "  F5 .... display samples list" CRLF
                "  F6 .... display debug information" CRLF
                "  P ..... pause playback (only on SB16)" CRLF
                "  L ..... enable/disable song loop." CRLF
                "  <Alt>+(<1>-<0>,<Q>-<R>) ... toggle channel 1-16 on/off." CRLF
                "  + ..... jump to next pattern" CRLF
                "  - ..... jump to previous pattern" CRLF
                "  D ..... DOS shell." CRLF
                "  ESC ... stop playback and exit" CRLF
            );
            textcolor (_white);
            printf (
                "Samples list:" CRLF
            );
            textcolor (_lightgray);
            printf (
                "  < ..... previous page." CRLF
                "  > ..... next page" CRLF
            );
        }
    }
}

/*
void __far win_help_keypress (SCRWIN *self, char key)
{
}

void __far win_help_free (SCRWIN *self)
{
}
*/
