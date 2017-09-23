/* vsprintf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdarg.h>

#include "pascal.h"

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

int cc_vsprintf(char *str, const char *format, va_list ap)
{
    DATASTREAM ds;
    dataStreamInitMemory(&ds, str, 0);
    _dsprintf(&ds, format, ap);
    str[ds.written] = 0;
    return ds.written;
}
