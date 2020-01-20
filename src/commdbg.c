/* commdbg.c -- common library for debug messages.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$commdbg$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "hexdigts.h"
#include "cc/string.h"
#include "commdbg.h"

/*
 * Example:
 *      #define LEN 8
 *      char s [LEN + 1];
 *      _DEBUG_get_xnum (0xBAAD5EED, LEN, s);
 *      // s = "BAAD5EED";
 *
 *  Description:
 *      Destination must be of the size (len + 1) bytes to hold the result.
 */
void __far _DEBUG_get_xnum (uint32_t value, char len, char *dest)
{
    char *c, count;

    c = & (dest [len - 1]);
    count = len;
    while (count)
    {
        *c = HEXDIGITS [value & 15];
        value >>= 4;
        c--;
        count--;
    }
    dest[len] = 0;
}

/*
 * Example:
 *      #define MAX 16
 *      char s [MAX * 4 + 1];
 *      _DEBUG_get_xline (HEXDIGITS, 12, MAX, s);
 *      // s = "30 31 32 33 34 35 36 37 38 39 41 42             0123456789AB";
 *
 *  Description:
 *      Destination must be of the size (max * 4 + 1) bytes to hold the result.
 */
void __far _DEBUG_get_xline (void *buf, uint8_t size, uint8_t max, char *dest)
{
    char *p;
    int i;

    p = (char *) buf;
    i = size;
    while (i)
    {
        _DEBUG_get_xnum (*p, 2, dest);
        dest [2] = ' ';
        p++;
        dest += 3;
        i--;
    }

    if (max - size)
    {
        memset (dest, ' ', (max - size) * 3);
        dest += (max - size) * 3;
    }

    p = (char *) buf;
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
