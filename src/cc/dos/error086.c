/* error086.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$error086$*"
#endif

#include <stddef.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/errno.h"
#include "cc/dos.h"
#include "cc/dos/dosret.h"
#include "cc/dos/error086.h"

#ifdef DEFINE_LOCAL_DATA

int _cc_doserrno;

#endif

unsigned __cc_doserror(union CC_REGPACK *regs)
{
    int olderr;
    struct CC_DOSERROR err;

    if (regs->w.flags & CC_INTR_CF)
    {
        olderr = _cc_doserrno;
        // FIXME: if DOS < 3.0, then use regs.w.ax
        if (!cc_dosexterr(&err))
            _cc_doserrno = err.exterror;
        else
            _cc_doserrno = olderr;
        return __cc_set_errno_dos(_cc_doserrno);
    }
    else
    {
        _cc_doserrno = 0;
        return CC_EZERO;
    }
}
