/* clrscr.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$conio$clrscr$*"
#endif

#include <stdbool.h>
#include "cc/i86.h"
#include "hw/vbios.h"
#include "cc/conio.h"

void cc_clrscr(void)
{
    vbios_scroll_up(cc_windmin.rect.x, cc_windmin.rect.y, cc_windmax.rect.x, cc_windmax.rect.y, 0, cc_textattr);
    vbios_set_cursor_pos(0, cc_windmin.rect.x, cc_windmin.rect.y);
}
