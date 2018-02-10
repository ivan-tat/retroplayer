/* getch.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>

#include "cc/i86.h"

#include "cc/conio.h"

char cc_getch(void)
{
    union REGPACK regs;

    regs.h.ah = 0;
    intr(0x16, &regs);

    if (regs.h.al)
        return regs.h.al;
    else
        return regs.h.ah;
}
