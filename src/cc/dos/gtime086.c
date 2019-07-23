/* gtime086.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$gtime086$*"
#endif

#include "cc/i86.h"
#include "cc/dos.h"

void _cc_dos_getdate(struct cc_dosdate_t *d)
{
    union CC_REGPACK regs;
    regs.h.ah = 0x2a;
    cc_intr(0x21, &regs);
    d->day = regs.h.dl;
    d->month = regs.h.dh;
    d->year = regs.w.cx;
    d->dayofweek = regs.h.al;
}

void _cc_dos_gettime(struct cc_dostime_t *t)
{
    union CC_REGPACK regs;
    regs.h.ah = 0x2c;
    cc_intr(0x21, &regs);
    t->hour = regs.h.ch;
    t->minute = regs.h.cl;
    t->second = regs.h.dh;
    t->hsecond = regs.h.dl;
}
