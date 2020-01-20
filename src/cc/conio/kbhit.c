/* kbhit.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$conio$kbhit$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include "cc/i86.h"
#include "cc/conio.h"

bool cc_kbhit(void)
{
    union REGPACK regs;

    regs.h.ah = 1;
    intr(0x16, &regs);

    return (regs.w.flags & CC_INTR_ZF) == 0;
}
