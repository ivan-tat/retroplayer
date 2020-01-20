/* strtol.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$stdlib$strtol$*"
#endif  /* __WATCOMC__ */

#include <limits.h>
#include "cc/ctype.h"
#include "cc/errno.h"
#include "cc/stdlib.h"

#define _BigInt(base) LONG_MAX / base

static const long _maxvalue[35] =
{
    _BigInt(2),  _BigInt(3),  _BigInt(4),  _BigInt(5),  _BigInt(6),
    _BigInt(7),  _BigInt(8),  _BigInt(9),  _BigInt(10), _BigInt(11),
    _BigInt(12), _BigInt(13), _BigInt(14), _BigInt(15), _BigInt(16),
    _BigInt(17), _BigInt(18), _BigInt(19), _BigInt(20), _BigInt(21),
    _BigInt(22), _BigInt(23), _BigInt(24), _BigInt(25), _BigInt(26),
    _BigInt(27), _BigInt(28), _BigInt(29), _BigInt(30), _BigInt(31),
    _BigInt(32), _BigInt(33), _BigInt(34), _BigInt(35), _BigInt(36)
};

/* base in 2..36 */
#define _get_limit(base) _maxvalue[base - 2]

int __near _get_actual_base(const char **str, int base)
{
    char c[2];

    c[0] = ((const char *)*str)[0];
    c[1] = ((const char *)*str)[1];

    if ((base == 0) || (base == 16))
    {
        if ((c[0] == '0') && (c[1] == 'x'))
        {
            *str += 2;
            return 16;
        }
    }
    if (base == 0)
    {
        if ((c[0] == '0') && (c[1] == '0'))
        {
            *str += 2;
            return 8;
        }
        return 10;
    }

    return base;
}

int __near _get_digit(char c, int base)
{
    int value;

    if ((c >= '0') && (c <= '9'))
        value = c - '0';
    else
    if ((c >= 'A') && (c <= 'Z'))
        value = c - 'A' + 10;
    else
    if ((c >= 'a') && (c <= 'z'))
        value = c - 'a' + 10;
    else
        value = -1;

    if (value < base)
        return value;

    return -1;
}

long cc_strtol(const char *nptr, char **endptr, int base)
{
    const char *p, *signptr;
    char c;
    signed char digit;
    bool sign, negate, overflow;
    long limit, value;
    int count;

    if ((nptr == NULL) || (base < 0) || (base == 1) || (base > 36))
    {
        cc_errno = CC_EINVAL;
        return 0;
    }

    if (!*nptr)
    {
        if (endptr)
            *endptr = (char *)nptr;
        return 0;
    }

    p = nptr;

    /* skip spaces */

    while (true)
    {
        c = *p;
        if (!isspace(c))
            break;
        p++;
    }

    /* get sign */

    sign = false;
    negate = false;
    signptr = NULL;
    switch (c)
    {
        case '-':
            sign = true;
            negate = true;
            signptr = p;
            p++;
            break;

        case '+':
            sign = true;
            signptr = p;
            p++;
            break;

        default:
            break;
    }

    /* check radix */

    if ((base == 0) || (base == 16))
        base = _get_actual_base(&p, base);

    /* read digits */

    limit = _get_limit(base);
    if (negate)
        limit = -limit - 1L;
    value = 0;
    count = 0;
    overflow = false;

    p--;
    do
    {
        p++;
        c = *p;
        if (c)
        {
            digit = _get_digit(c, base);
            if (digit >= 0)
            {
                count++;
                if (negate)
                    overflow = (-value < limit);
                else
                    overflow = (value > limit);
                if (!overflow)
                {
                    value *= base;
                    value += digit;
                }
            }
        }
    }
    while (c && (digit >= 0) && !overflow);

/*
  _: spaces, +: sign, #: digits count, ^: overflow, C: char
    input    _ + # ^ C | value  endptr errno
    ""       0 0 0 0 0   0      &end   -
    " "      1 0 0 0 0   0      &end   -
    "+"      0 1 0 0 0   0      &sign  EINVAL
    " +"     1 1 0 0 0   0      &sign  EINVAL
    "c"      0 0 0 0 c   0      &c     EINVAL
    " c"     1 0 0 0 c   0      &c     EINVAL
    "+c"     0 1 0 0 c   0      &c     EINVAL
    " +c"    1 1 0 0 c   0      &c     EINVAL
    "dc"     0 0 N 0 c   d      &c     -
    " dc"    1 0 N 0 c   d      &c     -
    "+dc"    0 1 N 0 c   d      &c     -
    " +dc"   1 1 N 0 c   d      &c     -
    "d"      0 0 N 0 0   d      &end   -
    " d"     1 0 N 0 0   d      &end   -
    "+d"     0 1 N 0 0   d      &end   -
    " +d"    1 1 N 0 0   d      &end   -
    "dD"     0 0 N 1 D   limit  &D     ERANGE
    " dD"    1 0 N 1 D   limit  &D     ERANGE
    "+dD"    0 1 N 1 D   limit  &D     ERANGE
    " +dD"   1 1 N 1 D   limit  &D     ERANGE

  Optimized:
  1 " +dD"   * * N 1 D   limit  &D     ERANGE
  2 " +dc"   * * N 0 c   d      &c     -
  3 " +d"    * * N 0 0   d      &end   -
  4 " +c"    * * 0 0 c   0      &c     EINVAL
  5 " +"     * 1 0 0 0   0      &sign  EINVAL
  6 " "      * 0 0 0 0   0      &end   -

  1,2,3,4,5,6: if (oveflow) { 1 } else { 2,3,4,5,6 }
  2,3,4,5,6:   if (count) { 2,3 } else { 4,5,6 }
  2,3:         if (c) { 2 } else { 3 }
  4,5,6:       if (c) { 4 } else { 5,6 }
  5,6:         if (sign) { 5 } else { 6 }
*/
    if (overflow)
    {
        /* 1 */
        cc_errno = CC_ERANGE;

        if (endptr)
            *endptr = (char *)p;

        value = LONG_MAX;

        if (negate)
            value = -value - 1L;

        negate = false;
    }
    else
        if (count)
        {
            /* 2,3 */
            if (endptr)
                *endptr = (char *)p;
        }
        else
        {
            if (c)
                /* 4 */
                cc_errno = CC_EINVAL;
            else
                /* 5,6 */
                if (sign)
                {
                    cc_errno = CC_EINVAL;
                    p = signptr;
                }

            if (endptr)
                *endptr = (char *)p;

            value = 0;
            negate = false;
        }

    if (negate)
        value = -value;

    return value;
}
