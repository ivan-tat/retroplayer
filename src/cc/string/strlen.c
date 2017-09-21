/* strlen.c -- part of custom "string" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include "cc/string.h"

size_t cc_strlen(const char *s)
{
    register const char *ptr;
    register size_t count;

    if (s)
    {
        ptr = s;
        count = ~0;
        while (*ptr && count)
        {
            ptr++;
            count--;
        };

        return ~count;
    };

    return 0;
}
