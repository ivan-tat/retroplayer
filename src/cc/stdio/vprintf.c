/* vprintf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdarg.h>

#include "pascal.h"
#include "dstream.h"

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

#define BUFSIZE 128

int cc_vprintf(const char *format, va_list ap)
{
    char buf[BUFSIZE];
    DATASTREAM ds;
    datastream_init (&ds, DSFLAG_BUFFER, &_system_flush_stdout);
    datastream_set_buf_size (&ds, BUFSIZE - 1);    /* (-1) for terminating zero */
    datastream_set_buf_ptr (&ds, &buf);
    _printf (&ds, format, ap);
    return ds.written;
}
