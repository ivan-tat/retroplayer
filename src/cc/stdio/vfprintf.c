/* vfprintf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdarg.h>

#include "pascal/pascal.h"

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

#define BUFSIZE 128

int cc_vfprintf(FILE *stream, const char *format, va_list ap)
{
    char buf[BUFSIZE];
    DATASTREAM ds;
    dataStreamInitFile(&ds, stream, &buf, BUFSIZE);
    _dsprintf(&ds, format, ap);
    return ds.written;
}
