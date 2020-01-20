/* dosterm.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$dosterm$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "cc/i86.h"
#include "cc/dos.h"

// No return.
void _cc_dos_terminate(uint8_t code)
{
    union CC_REGPACK regs;
    regs.h.al = code;
    regs.h.ah = 0x4c;
    cc_intr(0x21, &regs);
}
