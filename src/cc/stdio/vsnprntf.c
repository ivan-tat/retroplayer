/* vsnprntf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdarg.h>

#include "pascal/pascal.h"

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

int cc_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    DATASTREAM ds;
    dataStreamInitMemory(&ds, str, size);
    _dsprintf(&ds, format, ap);
    str[ds.written] = 0;
    return ds.written;
}
