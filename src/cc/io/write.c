/* write.c -- part of custom "io" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$io$write$*"
#endif

#include "cc/dos.h"
#include "cc/io.h"

int32_t cc_write(int fd, void *buf, size_t count)
{
    uint16_t actual;

    return _cc_dos_write(fd, buf, count, &actual) ? -1 : actual;
}
