/* window.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>

#include "cc/i86.h"
#include "hw/vbios.h"

#include "cc/conio.h"

/*
 * Description:
 *      All input coordinates are 1-based.
 */
void cc_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    if ((0 < x0) && (x0 <= x1)
    &&  (0 < y0) && (y0 <= y1)
    &&  (x1 <= cc_screenwidth)
    &&  (y1 <= cc_screenheight))
    {
        cc_windmin.rect.x = x0 - 1;
        cc_windmin.rect.y = y0 - 1;
        cc_windmax.rect.x = x1 - 1;
        cc_windmax.rect.y = y1 - 1;
        cc_gotoxy(1, 1);
    }
}
