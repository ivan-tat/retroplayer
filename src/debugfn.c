/* debugfn.c -- helper functions for debug library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>
#include "cc/string.h"

#include "debugfn.h"

static const char _hexdigits[16] = "0123456789ABCDEF";

/*
 * Example:
 *      #define LEN 8
 *      char s[LEN+1];
 *      _DEBUG_get_xnum(0xBAAD5EED, LEN, s);
 *      // s = "BAAD5EED";
 *
 *  Description:
 *      Destination must be of the size (len+1) bytes to hold the result.
 */
void __far _DEBUG_get_xnum(uint32_t value, char len, char *dest)
{
    char *c, count;

    c = &(dest[len - 1]);
    count = len;
    while (count)
    {
        *c = _hexdigits[value & 15];
        value >>= 4;
        c--;
        count--;
    }
    dest[len] = 0;
}

/*
 * Example:
 *      #define MAX 16
 *      char s[MAX*4+1];
 *      _DEBUG_get_xline(_hexdigits, 12, MAX, s);
 *      // s = "30 31 32 33 34 35 36 37 38 39 41 42 43          0123456789ABC";
 *
 *  Description:
 *      Destination must be of the size (max*4+1) bytes to hold the result.
 */
void __far _DEBUG_get_xline(void *buf, uint8_t size, uint8_t max, char *dest)
{
    char *p;
    int i;

    p = (char *)buf;
    i = size;
    while (i)
    {
        _DEBUG_get_xnum(*p, 2, dest);
        dest[2] = ' ';
        p++;
        dest += 3;
        i--;
    }

    if (max - size)
    {
        memset(dest, ' ', (max - size) * 3);
        dest += (max - size) * 3;
    }

    p = (char *)buf;
    i = size;
    while (i)
    {
        *dest = (*p < 32) ? '.' : *p;
        p++;
        dest++;
        i--;
    }

    *dest = 0;
}

#define LINE_SIZE 16
void __far _DEBUG_dump_mem(void *buf, unsigned size, const char *padstr)
{
    const char *p;
    unsigned o, left;
    char len;
    char s[4 + 1 + LINE_SIZE * 4 + 1];

    p = (char *)buf;
    o = 0;
    left = size;
    while (left)
    {
        len = (left > LINE_SIZE) ? LINE_SIZE : left;
        _DEBUG_get_xnum(o, 4, s);
        s[4] = ' ';
        _DEBUG_get_xline((void *)p, len, LINE_SIZE, &(s[5]));
        if (padstr) _DEBUG_print(padstr);
        _DEBUG_print(s);
        _DEBUG_print(CRLF);
        p += len;
        o += len;
        left -= len;
    }
}
