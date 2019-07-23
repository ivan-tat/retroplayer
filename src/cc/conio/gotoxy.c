/* gotoxy.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$conio$gotoxy$*"
#endif

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

    if (x && y)
    {
        sx = cc_windmin.rect.x + x - 1;
        if ((sx >= cc_windmin.rect.x) && (sx <= cc_windmax.rect.x))
        {
            sy = cc_windmin.rect.y + y - 1;
            if ((sy >= cc_windmin.rect.y) && (sy <= cc_windmax.rect.y))
                vbios_set_cursor_pos(0, sx, sy);
        }
    }
}
