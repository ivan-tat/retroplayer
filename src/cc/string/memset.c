/* memset.c -- part of custom "string" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$string$memset$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "cc/string.h"

void *cc_memset(void *s, int c, size_t n)
{
    register char *ptr;
    register size_t count;

    if (n)
    {
        ptr = s;
        count = n;
        do
        {
            *ptr = c;
            ptr++;
            count--;
        } while (count);
    }

    return s;
}
