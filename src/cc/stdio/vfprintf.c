/* vfprintf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdarg.h>

#include "pascal.h"
#include "dstream.h"

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

#define BUFSIZE 128

int cc_vfprintf(FILE *stream, const char *format, va_list ap)
{
    char buf[BUFSIZE];
    DATASTREAM ds;
    datastream_init (&ds, DSFLAG_BUFFER, &_system_flush_file);
    datastream_set_buf_size (&ds, BUFSIZE);
    datastream_set_buf_ptr (&ds, &buf);
    datastream_set_output (&ds, stream);
    _printf (&ds, format, ap);
    return ds.written;
}
