/* error086.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/errno.h"
#include "cc/dos.h"
#include "cc/dos/dosret.h"
#include "cc/dos/error086.h"

#ifdef DEFINE_LOCAL_DATA

int _cc_doserrno;

#endif

/*
 * Requirements:
 *      DOS 3.0+
 *
 * Description:
 *      AX = extended error code
 *      BH = error class
 *      BL = suggested action
 *      CH = location (where the error occured)
 */
unsigned __near __geterrorinfo(void)
{
    union CC_REGPACK regs;

    regs.h.ah = 0x59;
    regs.w.bx = 0;
    cc_intr(0x21, &regs);
    return regs.w.ax;
}

unsigned __cc_doserror(union CC_REGPACK *regs)
{
    if (regs->w.flags & CC_INTR_CF)
    {
        _cc_doserrno = __geterrorinfo();    // FIXME: if DOS < 3.0, then use regs.w.ax
        return __cc_set_errno_dos(_cc_doserrno);
    }
    else
    {
        _cc_doserrno = 0;
        return CC_EZERO;
    }
}
