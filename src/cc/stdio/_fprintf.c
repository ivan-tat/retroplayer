/* _fprintf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdio$_fprintf$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "cc/dstream.h"
#include "cc/stdio.h"
#include "cc/stdio/_fprintf.h"

bool __far _datastream_flush_file (DATASTREAM *self)
{
    if (self->output)
        return (fwrite (self->buf, self->pos, 1, (FILE *) self->output)) == 1;
    else
        return false;
}
