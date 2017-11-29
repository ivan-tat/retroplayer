/* pascal.c -- simple pascal library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>
#include "cc/i86.h"

#include "pascal.h"

void PUBLIC_CODE strpastoc(char *dest, char *src, uint16_t maxlen)
{
    uint16_t len;

    if (maxlen)
    {
        len = src[0];
        if (len > maxlen - 1)
            len = maxlen - 1;

        if (len)
            pascal_move(src + 1, dest, len);

        if (maxlen - len)
            pascal_fillchar(dest + len, maxlen - len, 0);
    }
}

void PUBLIC_CODE strctopas(char *dest, char *src, uint16_t maxlen)
{
    char *endptr;
    uint16_t len;

    if (maxlen)
    {
        endptr = src;
        while (endptr[0] != 0)
            endptr++;
        len = endptr - src;

        if (maxlen > 256)
            maxlen = 256;
        if (len > maxlen - 1)
            len = maxlen - 1;

        if (len)
            pascal_move(src, dest + 1, len);

        dest[0] = len;
        len++;
        if (maxlen - len)
            pascal_fillchar(dest + len, maxlen - len, 0);
    }
}
