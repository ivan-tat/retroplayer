/* pascal.c -- simple pascal library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>
#include "cc/i86.h"

#include "pascal.h"

void __far __pascal strpastoc (char *dest, char const *src, uint16_t maxlen)
{
    uint16_t len;

    if (maxlen)
    {
        len = src[0];
        if (len > maxlen - 1)
            len = maxlen - 1;

        if (len)
            pascal_move((void *)(src + 1), dest, len);

        if (maxlen - len)
            pascal_fillchar((void *)(dest + len), maxlen - len, 0);
    }
}

void __far __pascal strctopas (char *dest, char const *src, uint16_t maxlen)
{
    char const *endptr;
    uint16_t len;

    if (maxlen)
    {
        endptr = src;
        while (endptr[0] != 0)
            endptr++;
        len = endptr - src;

        if (maxlen > pascal_String_size)
            maxlen = pascal_String_size;
        if (len > maxlen - 1)
            len = maxlen - 1;

        if (len)
            pascal_move((void *)src, dest + 1, len);

        dest[0] = len;
        len++;
        if (maxlen - len)
            pascal_fillchar(dest + len, maxlen - len, 0);
    }
}
