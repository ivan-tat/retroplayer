/* error086.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/dos.h"
#include "cc/dos/dosret.h"
#include "cc/dos/error086.h"

unsigned __cc_doserror(union CC_REGPACK *regs)
{
    if (regs->w.flags & CC_INTR_CF)
        return __cc_set_errno_dos(regs->w.ax);
    else
        return 0;
}
