/* error086.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$error086$*"
#endif  /* __WATCOMC__ */

#include <stddef.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/errno.h"
#include "cc/dos.h"
#include "cc/dos/dosret.h"
#include "cc/dos/error086.h"

#if DEFINE_LOCAL_DATA == 1

int _cc_doserrno;

#endif  /* DEFINE_LOCAL_DATA == 1 */

/*
 * Description:
 *      This methods checks REGPACK structure before changing "_doserrno"
 *      variable. Returns "_doserrno" value for known error or -1 if unknown.
 *      May set "errno" variable.
 */
unsigned __cc_doserror (union CC_REGPACK *regs)
{
    if (regs->w.flags & CC_INTR_CF)
        return __cc_doserror2 ();
    else
    {
        _cc_doserrno = 0;
        return CC_EZERO;
    }
}

/*
 * Description:
 *      This methods assumes that there was a DOS error and may change
 *      "_doserrno" variable. Useful when calling DOS services directly
 *      without the use of REGPACK structure. In this case you should set
 *      "_doserrno" variable manually before call. If the error code is known
 *      then it sets "errno" variable.
 */
unsigned __cc_doserror2 (void)
{
    int olderr;
    struct CC_DOSERROR err;

    olderr = _cc_doserrno;
    // FIXME: if DOS < 3.0, then use regs.w.ax
    if (cc_dosexterr (&err) == CC_EZERO)
        _cc_doserrno = err.exterror;
    else
        _cc_doserrno = olderr;
    return __cc_set_errno_dos (_cc_doserrno);
}
