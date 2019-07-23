/* fsetpos.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdio$fsetpos$*"
#endif

#include "pascal.h"
#include "startup.h"
#include "cc/i86.h"
#include "cc/errno.h"
#include "cc/dos.h"
#include "cc/string.h"
#include "cc/io.h"
#include "cc/stdio.h"

int cc_fsetpos(FILE *stream, fpos_t pos)
{
    int result;

    if (stream == NULL)
    {
        cc_errno = CC_EINVAL;
        return 0;
    }

    switch (stream->mode)
    {
    case cc_fmClosed:
        cc_errno = CC_EBADF;
        cc_InOutRes = EINOUTRES_NOT_OPENED;
        return 0;
    case cc_fmInput:
    case cc_fmOutput:
    case cc_fmInOut:
        result = cc_lseek(stream->handle, pos, CC_SEEK_SET);
        if (result == -1)
        {
            cc_InOutRes = _cc_doserrno;
            return -1;
        }
        else
        {
            cc_InOutRes = EINOUTRES_SUCCESS;
            return 0;
        }
    default:
        cc_errno = CC_EBADF;
        cc_InOutRes = EINOUTRES_NOT_ASSIGNED;
        return 0;
    }
    return -1;
}
