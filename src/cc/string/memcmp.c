/* memcmp.c -- part of custom "string" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>
#include "cc/string.h"

int cc_memcmp(const void *s1, const void *s2, size_t n)
{
    register const char *ptr1;
    register const char *ptr2;
    register size_t count;
    register int res;

    if (n)
    {
        ptr1 = s1;
        ptr2 = s2;
        count = n;
        do
        {
            res = *ptr1 - *ptr2;
            ptr1++;
            ptr2++;
            count--;
        } while ((!res) && count);
        return res;
    };

    return 0;
}
