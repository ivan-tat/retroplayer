/* dosexter.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/errno.h"
#include "cc/dos.h"
#include "cc/dos/dosret.h"
#include "cc/dos/dosret.h"

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
int cc_dosexterr(struct CC_DOSERROR *err)
{
    union CC_REGPACK regs;

    if (!err)
    {
        cc_errno = CC_EINVAL;
        return -1;
    }

    regs.h.ah = 0x59;
    regs.w.bx = 0;
    cc_intr(0x21, &regs);
    if (regs.w.flags & CC_INTR_CF)
    {
        _cc_doserrno = regs.w.ax;
        return __cc_set_errno_dos(_cc_doserrno);
    }
    else
    {
        err->exterror = regs.w.ax;
        err->errclass = regs.h.bh;
        err->action = regs.h.bl;
        err->locus = regs.h.ch;
        return CC_EZERO;
    }
}
