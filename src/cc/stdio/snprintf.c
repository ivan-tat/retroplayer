/* snprintf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdarg.h>

#include "pascal.h"
#include "dstream.h"

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

int cc_snprintf(char *str, size_t size, const char *format, ...)
{
    va_list ap;
    DATASTREAM ds;
    va_start(ap, format);
    datastream_init (&ds, DSFLAG_DIRECT, NULL);
    datastream_set_limit (&ds, size);
    datastream_set_output (&ds, str);
    _printf (&ds, format, ap);
    str[ds.written] = 0;
    va_end(ap);
    return ds.written;
}
