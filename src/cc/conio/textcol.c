/* textcol.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$conio$textcol$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "cc/i86.h"
#include "hw/vbios.h"
#include "cc/conio.h"

void cc_textbackground(uint8_t color)
{
    cc_textattr = (cc_textattr & (_blink | 0x0f)) | ((color & 0x07) << 4);
}

void cc_textcolor(uint8_t color)
{
    if (color & (_blink | 0x70))
        color = (color & 0x0f) | _blink;
    cc_textattr = (cc_textattr & 0x70) | color;
}

void cc_lowvideo(void)
{
    cc_textattr &= ~8;
}

void cc_highvideo(void)
{
    cc_textattr |= 8;
}

void cc_normvideo(void)
{
    cc_textattr = cc_textattrorig;
}
