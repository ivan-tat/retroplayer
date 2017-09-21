/* strchr.c -- part of custom "string" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "cc/string.h"

char *cc_strchr(const char *s, int c)
{
    register const char *ptr;
    register size_t count;

    if (s)
    {
        ptr = s;
        count = ~0;
        while (*ptr != c && count)
        {
            ptr++;
            count--;
        }

        if (count)
            return (char *)ptr;
    };

    return NULL;
}
