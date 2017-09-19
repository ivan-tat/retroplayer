/* dos_.c -- custom DOS library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <i86.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#endif

#include "pascal/pascal.h"
#include "ow/dos_.h"

uint16_t __near __doserror(uint16_t code)
{
    return code ? code : 0xffff;
}

uint16_t PUBLIC_CODE _dos_allocmem(uint16_t size, uint16_t *seg)
{
    union REGPACK regs;
    regs.w.bx = size;
    regs.h.ah = 0x48;
    intr(0x21, &regs);
    if (regs.w.flags & INTR_CF)
    {
        *seg = regs.w.bx;
        return __doserror(regs.w.ax);
    }
    else
    {
        *seg = regs.w.ax;
        return 0;
    };
}

uint16_t PUBLIC_CODE _dos_freemem(uint16_t seg)
{
    union REGPACK regs;
    regs.w.es = seg;
    regs.h.ah = 0x49;
    intr(0x21, &regs);
    if (regs.w.flags & INTR_CF)
        return __doserror(regs.w.ax);
    else
        return 0;
}

uint16_t PUBLIC_CODE _dos_setblock(uint16_t size, uint16_t seg, uint16_t *max)
{
    union REGPACK regs;
    regs.w.bx = size;
    regs.w.es = seg;
    regs.h.ah = 0x4a;
    intr(0x21, &regs);
    if (regs.w.flags & INTR_CF)
    {
        *max = regs.w.bx;
        return __doserror(regs.w.ax);
    }
    else
        return 0;
}
