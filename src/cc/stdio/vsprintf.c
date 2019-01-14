/* vsprintf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdarg.h>
#include "cc/dstream.h"
#include "cc/_printf.h"
#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

int cc_vsprintf(char *str, const char *format, va_list ap)
{
    DATASTREAM ds;
    datastream_init (&ds, DSFLAG_DIRECT, NULL);
    datastream_set_limit (&ds, 0);
    datastream_set_output (&ds, str);
    _printf (&ds, format, ap);
    str[ds.written] = 0;
    return ds.written;
}
