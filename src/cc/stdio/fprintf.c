/* fprintf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdarg.h>

#include "pascal.h"

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

#define BUFSIZE 128

int cc_fprintf(FILE *stream, const char *format, ...)
{
    va_list ap;
    char buf[BUFSIZE];
    DATASTREAM ds;
    va_start(ap, format);
    datastream_init (&ds, DSFLAG_BUFFER, &_system_flush_file);
    datastream_set_buf_size (&ds, BUFSIZE);
    datastream_set_buf_ptr (&ds, &buf);
    datastream_set_output (&ds, stream);
    _printf (&ds, format, ap);
    va_end(ap);
    return ds.written;
}
