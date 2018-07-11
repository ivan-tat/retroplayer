/* lseek.c -- part of custom "io" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "cc/dos.h"
#include "cc/io.h"

int32_t cc_lseek(int fd, int32_t offset, int whence)
{
    int32_t newoffset;

    return _cc_dos_seek(fd, offset, whence, &newoffset) ? -1 : newoffset;
}
