/* coniodos.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$coniodos$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "cc/i86.h"
#include "cc/dos.h"

/*
 * Performs a console write to standard output.
 * Does not check for Ctrl-Break.
 */
void _cc_dos_console_out(uint8_t c)
{
    union CC_REGPACK regs;
    if (c != 0xff)
    {
        regs.h.dl = c;
        regs.h.ah = 6;
        cc_intr(0x21, &regs);
    }
}

/*
 * Performs a no wait console read from standard input if there is a character ready.
 * Does not check for Ctrl-Break. Call twice for extended ASCII character code.
 */
uint16_t _cc_dos_console_in(void)
{
    union CC_REGPACK regs;
    uint8_t c;
    regs.h.dl = 0xff;
    regs.h.ah = 6;
    cc_intr(0x21, &regs);
    if (regs.w.flags & CC_INTR_ZF)
        return 0;
    else
        return regs.h.al + 0x100;
}
