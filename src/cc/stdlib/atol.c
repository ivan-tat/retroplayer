/* atol.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$atol$*"
#endif

#include "cc/ctype.h"
#include "cc/stdlib.h"

long cc_atol(const char *src)
{
    const char *p;
    char c;
    bool negative;
    long value;

    p = src;
    while (isspace(c = *p)) p++;

    negative = false;

    switch (c)
    {
        case '-':
            negative = true;
            p++;
            break;
        case '+':
            p++;
            break;
        default:
            break;
    }

    value = 0;
    while (isdigit(c = *p++))
        value = (value * 10) + c - '0';

    return negative ? -value : value;
}
