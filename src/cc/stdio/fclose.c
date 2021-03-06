/* fclose.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdio$fclose$*"
#endif  /* __WATCOMC__ */

#include "pascal.h"
#include "startup.h"
#include "cc/i86.h"
#include "cc/errno.h"
#include "cc/dos.h"
#include "cc/string.h"
#include "cc/io.h"
#include "cc/stdio.h"

int cc_fclose(FILE *stream)
{
    if (stream == NULL)
    {
        cc_errno = CC_EINVAL;
        cc_InOutRes = EINOUTRES_NOT_OPENED;
        return -1;
    }

    switch (stream->mode)
    {
    case cc_fmClosed:
        cc_errno = CC_EINVAL;
        cc_InOutRes = EINOUTRES_NOT_OPENED;
        return -1;
    case cc_fmInput:
    case cc_fmOutput:
    case cc_fmInOut:
        if (stream->handle > 4)
        {
            if (!cc_close(stream->handle))
            {
                _cc_dos_freemem(FP_SEG(stream));
                cc_InOutRes = EINOUTRES_SUCCESS;
                return 0;
            }
            else
            {
                cc_InOutRes = _cc_doserrno;
                return -1;
            }
        }
        else
        {
            _cc_dos_freemem(FP_SEG(stream));
            cc_InOutRes = EINOUTRES_SUCCESS;
            return 0;
        }
    default:
        cc_errno = CC_EINVAL;
        cc_InOutRes = EINOUTRES_NOT_ASSIGNED;
        return -1;
    }
}
