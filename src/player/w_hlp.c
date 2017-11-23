/* w_hlp.c -- help window methods.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/conio.h"
#include "player/screen.h"

#include "player/w_hlp.h"

void __far win_help_init(SCRWIN *self)
{
    scrwin_init(
        self,
        1, 6, scrWidth, scrHeight,
        &win_help_draw,
        NULL
    );
}

void __far win_help_draw(SCRWIN *self)
{
    if (scrwin_is_created(self))
    {
        textbackground(_black);
        textcolor(_lightgray);

        if (scrwin_get_flags(self) & WINFL_FULLREDRAW)
        {
            clrscr();
            printf(
                "  Keys while playing:" CRLF
                CRLF
                "  F1 .... this help screen" CRLF
                "  F2 .... display channels information" CRLF
                "  F3 .... display current pattern" CRLF
                "  F4 .... display instruments information" CRLF
                "  F5 .... display samples memory positions" CRLF
                "  F6 .... display debug information" CRLF
                "  P ..... pause playback (only on SB16)" CRLF
                "  L ..... enable/disable song loop." CRLF
                "  <Alt>+(<1>-<0>,<Q>-<R>) ... toggle channel 1-16 on/off." CRLF
                "  + ..... jump to next pattern." CRLF
                "  - ..... jump to previous pattern" CRLF
                "  D ..... DOS shell." CRLF
                "  ESC ... stop playback and exit."
            );
        }
    }
}
