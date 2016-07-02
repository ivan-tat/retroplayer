/* dosproc.c -- DOS memory management.

   This is free and unencumbered software released into the public domain */

#ifdef __WATCOMC__
#include <i86.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#endif

bool __far __pascal getdosmem( void **p, uint32_t size ) {
    union REGPACK regs;
    regs.w.bx = (size+15)>>4;
    regs.h.ah = 0x48;
    intr( 0x21, &regs );
    if (regs.w.flags & INTR_CF) {
        return false;
    } else {
        *p = MK_FP(regs.w.ax, 0);
        return true;
    };
}

void __far __pascal freedosmem( void **p ) {
    union REGPACK regs;
    regs.h.ah = 0x49;
    regs.w.di = FP_OFF(*p);
    regs.w.es = FP_SEG(*p);
    intr( 0x21, &regs );
    *p = MK_FP(0, 0);
}

bool __far __pascal setsize( void **p, uint32_t size ) {
    union REGPACK regs;
    regs.h.ah = 0x4a;
    regs.w.bx = (size+15)>>4;
    regs.w.di = FP_OFF(*p);
    regs.w.es = FP_SEG(*p);
    intr( 0x21, &regs );
    if (regs.w.flags & INTR_CF) {
        return false;
    } else {
        *p = MK_FP(regs.w.ax, 0);
        return true;
    };
}

uint16_t __far __pascal getfreesize( void ) {
    union REGPACK regs;
    regs.h.ah = 0x48;
    regs.w.bx = 0xffff;
    intr( 0x21, &regs );
    return regs.w.bx;
}
