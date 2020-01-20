/* close.c -- part of custom "io" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$io$close$*"
#endif  /* __WATCOMC__ */

#include "cc/dos.h"
#include "cc/io.h"

int cc_close(int fd)
{
    return _cc_dos_close(fd) ? -1 : 0;
}
