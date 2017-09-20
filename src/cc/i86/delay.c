/* delay.c -- part of custom "i86" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal/pascal.h"
#include "cc/i86.h"

void cc_delay(unsigned int __milliseconds)
{
    union CC_REGPACK regs;
    unsigned long ms;

    ms = (unsigned long)__milliseconds * 1000;
    regs.h.ah = 0x86;
    regs.w.cx = ms & 0xffff;
    regs.w.dx = ms >> 16;
    cc_intr(0x15, &regs);
}
