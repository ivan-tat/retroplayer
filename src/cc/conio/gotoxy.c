/* gotoxy.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>

#include "cc/i86.h"
#include "hw/vbios.h"

#include "cc/conio.h"

/*
 * Description:
 *      "x" and "y" are 1-based.
 */
void cc_gotoxy(uint8_t x, uint8_t y)
{
    int sx, sy;
    sx = windmin.rect.x + x - 1;
    if ((sx >= windmin.rect.x) && (sx <= windmax.rect.x))
    {
        sy = windmin.rect.y + y - 1;
        if ((sy >= windmin.rect.y) && (sy <= windmax.rect.y))
            vbios_set_cursor_pos(0, sx, sy);
    }
}
