/* mem086.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"

#ifndef USE_INTRINSICS
/*
unsigned _cc_dos_para(unsigned size)
*/
uint16_t PUBLIC_CODE _cc_dos_para(uint16_t size)
{
    return __cc_dos_para(size);
}

#endif  /* !USE_INTRINSICS */

/*
unsigned _cc_dos_allocmem(unsigned size, unsigned *seg)
*/
uint16_t PUBLIC_CODE _cc_dos_allocmem(uint16_t size, uint16_t *seg)
{
    union CC_REGPACK regs;

    regs.w.bx = size;
    regs.h.ah = 0x48;
    cc_intr(0x21, &regs);
    if (regs.w.flags & CC_INTR_CF)
    {
        *seg = regs.w.bx;
        return __cc_doserror(regs.w.ax);
    }
    else
    {
        *seg = regs.w.ax;
        return 0;
    };
}

/*
unsigned _cc_dos_freemem(unsigned seg)
*/
uint16_t PUBLIC_CODE _cc_dos_freemem(uint16_t seg)
{
    union CC_REGPACK regs;

    regs.w.es = seg;
    regs.h.ah = 0x49;
    cc_intr(0x21, &regs);
    if (regs.w.flags & CC_INTR_CF)
        return __cc_doserror(regs.w.ax);
    else
        return 0;
}

/*
unsigned _cc_dos_setblock(unsigned size, unsigned seg, unsigned *max)
*/
uint16_t PUBLIC_CODE _cc_dos_setblock(uint16_t size, uint16_t seg, uint16_t *max)
{
    union CC_REGPACK regs;

    regs.w.bx = size;
    regs.w.es = seg;
    regs.h.ah = 0x4a;
    cc_intr(0x21, &regs);
    if (regs.w.flags & CC_INTR_CF)
    {
        *max = regs.w.bx;
        return __cc_doserror(regs.w.ax);
    }
    else
        return 0;
}
