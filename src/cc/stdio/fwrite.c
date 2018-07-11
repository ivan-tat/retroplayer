/* fwrite.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/errno.h"
#include "cc/dos.h"
#include "cc/string.h"
#include "cc/io.h"

#include "cc/stdio.h"

size_t cc_fwrite(void *ptr, size_t size, size_t n, FILE *stream)
{
    size_t actual;

    if ((size == 0) || (n == 0) || (stream == NULL))
    {
        cc_errno = CC_EINVAL;
        return 0;
    }

    switch (stream->mode)
    {
    case pascal_fmClosed:
        cc_errno = CC_EBADF;
        pascal_InOutRes = EINOUTRES_NOT_OPENED;
        return 0;
    case pascal_fmInput:
        cc_errno = CC_EACCES;
        pascal_InOutRes = EINOUTRES_NOT_OUTPUT;
        return 0;
    case pascal_fmOutput:
    case pascal_fmInOut:
        // FIXME: 64KiB limit for now
        actual = cc_write(stream->handle, ptr, size * n);
        if (actual > 0)
        {
            pascal_InOutRes = EINOUTRES_SUCCESS;
            return actual / size;
        }
        {
            pascal_InOutRes = _cc_doserrno;
            return 0;
        }
    default:
        cc_errno = CC_EBADF;
        pascal_InOutRes = EINOUTRES_NOT_ASSIGNED;
        return 0;
    }
}
