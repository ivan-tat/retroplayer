/* crt.c -- part of custom "conio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$conio$crt$*"
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "sysdbg.h"
#include "startup.h"
#include "cc/conio.h"

cc_inoutres_t __far _crt_open (_cc_iobuf *f);
cc_inoutres_t __far _crt_read (_cc_iobuf *f);
cc_inoutres_t __far _crt_write (_cc_iobuf *f);
cc_inoutres_t __far _crt_dummy (_cc_iobuf *f);

void __far cc_TextAssignCrt (_cc_iobuf *f, void *buffer, uint16_t size)
{
    SYSDEBUG_INFO ("Called.");
    f->mode = cc_fmClosed;
    f->buf_size = size;
    f->buf_ptr = buffer;
    f->io.by_name.open = _crt_open;
    f->name[0] = 0;
}

cc_inoutres_t __far _crt_open (_cc_iobuf *f)
{
    return EINOUTRES_SUCCESS;
}

cc_inoutres_t __far _crt_read (_cc_iobuf *f)
{
    return EINOUTRES_SUCCESS;
}

cc_inoutres_t __far _crt_write (_cc_iobuf *f)
{
    return EINOUTRES_SUCCESS;
}

cc_inoutres_t __far _crt_dummy (_cc_iobuf *f)
{
    return EINOUTRES_SUCCESS;
}
