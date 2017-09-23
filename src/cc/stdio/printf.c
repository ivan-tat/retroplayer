/* printf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdarg.h>

#include "pascal.h"

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

#define BUFSIZE 128

int cc_printf(const char *format, ...)
{
    va_list ap;
    char buf[BUFSIZE];
    DATASTREAM ds;
    va_start(ap, format);
    dataStreamInitStdOut(&ds, &buf, BUFSIZE);
    _dsprintf(&ds, format, ap);
    va_end(ap);
    return ds.written;
}
