/* mem086.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$mem086$*"
#endif

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"

#ifndef USE_INTRINSICS
/*
unsigned _cc_dos_para(unsigned size)
*/
uint16_t __far _cc_dos_para (uint32_t size)
{
    return __cc_dos_para(size);
}

#endif  /* !USE_INTRINSICS */

/*
unsigned _cc_dos_allocmem(unsigned size, unsigned *seg)
*/
uint16_t __far _cc_dos_allocmem (uint16_t size, uint16_t *seg)
{
    union CC_REGPACK regs;
    uint16_t error;

    regs.w.bx = size;
    regs.h.ah = 0x48;
    cc_intr(0x21, &regs);
    error = __cc_doserror(&regs);
    if (error)
        *seg = regs.w.bx;
    else
        *seg = regs.w.ax;
    return error;
}

/*
unsigned _cc_dos_freemem(unsigned seg)
*/
uint16_t __far _cc_dos_freemem (uint16_t seg)
{
    union CC_REGPACK regs;

    regs.w.es = seg;
    regs.h.ah = 0x49;
    cc_intr(0x21, &regs);
    return __cc_doserror(&regs);
}

/*
unsigned _cc_dos_setblock(unsigned size, unsigned seg, unsigned *max)
*/
uint16_t __far _cc_dos_setblock (uint16_t size, uint16_t seg, uint16_t *max)
{
    union CC_REGPACK regs;
    uint16_t error;

    regs.w.bx = size;
    regs.w.es = seg;
    regs.h.ah = 0x4a;
    cc_intr(0x21, &regs);
    error = __cc_doserror(&regs);
    if (error)
        *max = regs.w.bx;
    return error;
}
