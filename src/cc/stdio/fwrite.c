/* fwrite.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/string.h"

#include "cc/stdio.h"
#include "cc/stdio/_io.h"

size_t cc_fwrite(void *ptr, size_t size, size_t n, FILE *stream)
{
    size_t s;
    uint16_t actual;
    s = size * n;
    if (stream)
    {
        if (s)
        {
            if (pascal_blockwrite(stream, ptr, s, &actual))
                return n;
            else
                return actual / size;
        };
    };
    return 0;
}
