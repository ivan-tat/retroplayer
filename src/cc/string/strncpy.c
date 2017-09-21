/* strncpy.c -- part of custom "string" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stddef.h>
#include "cc/string.h"

char *cc_strncpy(char *dest, const char *src, size_t n)
{
    register char *ptr1;
    register const char *ptr2;
    register size_t count;
    bool next;

    if (dest && n)
    {
        ptr1 = dest;
        count = n;

        if (src)
        {
            ptr2 = src;
            do
            {
                *ptr1 = *ptr2;
                next = *ptr2 != 0;
                ptr1++;
                ptr2++;
                count--;
            } while (next && count);
        };

        while (count)
        {
            ptr1 = 0;
            ptr1++;
            count--;
        };
    };

    return dest;
}
