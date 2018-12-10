/* stricmp.c -- part of custom "string" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdbool.h>

#include "cc/string.h"

#define tolower(c) (((c >= 'A') && (c <= 'Z')) ? (c - 'A' + 'a') : c)

int cc_stricmp(const char *s1, const char *s2)
{
    register const char *ptr1;
    register const char *ptr2;
    register size_t count;
    register int res;
    bool next;

    res = 0;

    if (s1 && s2)
    {
        ptr1 = s1;
        ptr2 = s2;
        count = ~0;
        do
        {
            res = tolower(*ptr1) - tolower(*ptr2);
            next = *ptr1 && *ptr2;
            ptr1++;
            ptr2++;
            count--;
        } while ((!res) && next && count);
    }

    return res;
}
