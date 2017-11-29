/* getenv.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdbool.h>

#include "pascal.h"
#include "common.h"

#include "cc/stdlib.h"

extern void __near __pascal pascal_getenv(char *dest, const char *name);

#ifdef __WATCOMC__
#pragma aux pascal_getenv modify [ax bx cx dx si di es];
#endif

void *__near _memcpy(void *dest, const void *src, size_t n)
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

size_t __near _strlen(const char *s)
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
        }

        return ~count;
    }

    return 0;
}

char *__near _strncpy(char *dest, const char *src, size_t n)
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
        }

        while (count)
        {
            ptr1 = 0;
            ptr1++;
            count--;
        }
    }

    return dest;
}

char *custom_getenv(char *dest, const char *name, size_t maxlen)
{
    char _n[256];
    char _s[256];
    size_t len;

    _strncpy(&_n[1], name, 255);
    len = _strlen(name);
    if (len > 255)
        len = 255;
    _n[0] = len;

    pascal_getenv(_s, _n);

    strpastoc(dest, _s, maxlen);
    len = _s[0];
    if (len >= maxlen)
        len = maxlen-1;
    if (len > 0)
        _memcpy(dest, &_s[1], len);
    dest[len] = 0;

    return dest;
}
