/* memcpy.c -- part of custom "string" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$string$memcpy$*"
#endif

#include <stdint.h>
#include "cc/string.h"

void *cc_memcpy(void *dest, const void *src, size_t n)
{
    register char *ptr1;
    register const char *ptr2;
    register size_t count;

    if ((dest != src) && n)
    {
        ptr1 = dest;
        ptr2 = src;
        count = n;
        do
        {
            *ptr1 = *ptr2;
            ptr1++;
            ptr2++;
            count--;
        } while (count);
    }

    return dest;
}
