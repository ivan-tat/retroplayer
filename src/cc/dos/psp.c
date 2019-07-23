/* psp.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$psp$*"
#endif

#include <stdint.h>
#include "cc/i86.h"
#include "cc/dos.h"

unsigned _cc_dos_getpsp(void)
{
    union CC_REGPACK regs;
    regs.h.ah = 0x62;
    cc_intr(0x21, &regs);
    return regs.w.bx;
}

unsigned _cc_dos_getmasterpsp(void)
{
    unsigned parent, psp;

    parent = _cc_dos_getpsp();
    do
    {
        psp = parent;
        parent = ((struct cc_dospsp_t *)MK_FP(psp, 0))->parent_seg;
    } while (psp != parent);
    return psp;
}
