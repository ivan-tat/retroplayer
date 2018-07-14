/* clrscr.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>

#include "cc/i86.h"
#include "hw/vbios.h"

#include "cc/conio.h"

void cc_clrscr(void)
{
    vbios_scroll_up(windmin.rect.x, windmin.rect.y, windmax.rect.x, windmax.rect.y, 0, textattr);
    vbios_set_cursor_pos(0, windmin.rect.x, windmin.rect.y);
}
