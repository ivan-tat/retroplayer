/* _printf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdio$_printf$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dstream.h"
#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

bool __far _datastream_flush_console (DATASTREAM *self)
{
    self->buf[self->pos] = 0;
    pascal_write (self->buf);
    return true;
}
