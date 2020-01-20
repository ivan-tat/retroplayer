/* d_getvec.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$d_getvec$*"
#endif  /* __WATCOMC__ */

#include "cc/i86.h"
#include "cc/dos.h"

void __far *_cc_dos_getvect(unsigned num)
{
    union CC_REGPACK regs;
    regs.h.al = num;
    regs.h.ah = 0x35;
    cc_intr(0x21, &regs);
    return MK_FP(regs.w.es, regs.w.bx);
}
