/* d_setvec.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "cc/i86.h"
#include "cc/dos.h"

void __far _cc_dos_setvect(unsigned num, void __far *p)
{
    union CC_REGPACK regs;
    regs.h.al = num;
    regs.h.ah = 0x25;
    regs.w.dx = FP_OFF(p);
    regs.w.ds = FP_SEG(p);
    cc_intr(0x21, &regs);
}
