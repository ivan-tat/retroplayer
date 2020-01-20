/* _printf.c -- custom _printf() function.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$_printf$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dstream.h"
#include "cc/_printf.h"

/*
 *  Flag characters support (after %):
 *
 *  #   no
 *  0   yes
 *  -   no
 *  ' ' no
 *  +   no
 *
 * Length modifiers:
 *
 *  hh  yes
 *  h   yes
 *  l   yes
 *  ll  yes
 *  L   no
 *  j   no
 *  z   no
 *  t   no
 *
 * Conversion specifiers:
 *
 *  o       no
 *  d, i    yes
 *  u       yes
 *  x, X    yes
 *  e, E    no
 *  f, F    no
 *  g, G    no
 *  a, A    no
 *  c       yes
 *  s       yes
 *  p       no
 *  n       no
 *  %       yes
 *
 * Multibyte forms are not supported.
 */

#define HEXDIGIT_LC(x) ((x) < 10 ? '0' + (x) : 'a' + (x) - 10)
#define HEXDIGIT_UC(x) ((x) < 10 ? '0' + (x) : 'A' + (x) - 10)

typedef uint8_t format_flags_t;
typedef format_flags_t FORMATFLAGS;

#define FORMATFL_ZEROPAD    (1 << 0)
#define FORMATFL_UPPERCASE  (1 << 1)
#define FORMATFL_NEGATE     (1 << 7)

bool __near write_uchar
(
    DATASTREAM *stream,
    const unsigned char __v
)
{
    return datastream_write (stream, &__v, 1);
}

bool __near write_udecimal
(
    DATASTREAM *stream,
    const unsigned long __v,
    const FORMATFLAGS   __f,
    const unsigned char __n
)
{
    unsigned long _value;
    unsigned char _count;
    char s[10];
    char *p;
    int len;

    p = s + 10;
    len = 0;
    _value = __v;
    _count = __n <= 10 ? __n : 10;
    do
    {
        if (_count)
            _count--;
        p--;
        if (_value || len == 0)
        {
            *p = '0' + (_value % 10);
            _value /= 10;
        }
        else
            *p = (__f & FORMATFL_ZEROPAD) ? '0' : ' ';
        len++;
    } while (_value || _count);
    if (__f & FORMATFL_NEGATE)
    {
        *--p = '-';
        len++;
    }

    return datastream_write (stream, p, len);
}

/* ------------------------------------------------------------------ */

bool __near write_decimal
(
    DATASTREAM *stream,
    const signed long   __v,
    FORMATFLAGS         __f,
    const unsigned char __n
)
{
    unsigned long _value;

    if (__v < 0)
    {
        _value = -__v;
        __f |= FORMATFL_NEGATE;
    }
    else
        _value = __v;

    return write_udecimal (stream, _value, __f, __n);
}

/* ------------------------------------------------------------------ */

bool __near write_hexadecimal
(
    DATASTREAM *stream,
    const unsigned long __v,
    const FORMATFLAGS   __f,
    const unsigned char __n
)
{
    unsigned long _value;
    unsigned char _count;
    char s[8];
    char *p;
    int len;

    p = s + 8;
    len = 0;
    _value = __v;
    _count = __n <= 8 ? __n : 8;
    do
    {
        if (_count)
            _count--;
        p--;
        if (_value || len == 0)
        {
            *p = (__f & FORMATFL_UPPERCASE) ? HEXDIGIT_UC(_value & 15) : HEXDIGIT_LC(_value & 15);
            _value >>= 4;
        }
        else
            *p = (__f & FORMATFL_ZEROPAD) ? '0' : ' ';
        len++;
    } while (_value || _count);

    return datastream_write (stream, p, len);
}

/* ------------------------------------------------------------------ */

bool __far _printf (DATASTREAM *stream, const char *format, va_list ap)
{
    int i, j;
    bool ok;
    bool end;
    bool readnext;
    char c;
    /* Flag characters */
    FORMATFLAGS flags;
    /* Field width */
    unsigned char f_count;  /* number */
    /* Length modifiers */
    unsigned char f_half;   /* h, hh */
    unsigned char f_long;   /* l, ll */
    /* Values (internal)*/
    signed long   svalue;
    unsigned long uvalue;
    void         *pvalue;

    i = 0;
    j = 0;
    ok = true;
    end = false;
    while (ok && !end)
    {
        switch (c = format[i])
        {
            case '%':
                if (i - j > 0)
                {
                    ok = datastream_write (stream, format + j, i - j);
                    if (!ok)
                        break;
                }
                i++;
                flags = 0;
                f_count = 0;
                f_half = 0;
                f_long = 0;

                do
                {
                    readnext = false;
                    switch (c = format[i])
                    {
                        case '%':
                            ok = datastream_write (stream, "%", 1);
                            break;

                        case '0':
                            if (f_count == 0)
                            {
                                flags |= FORMATFL_ZEROPAD;
                                readnext = true;
                                break;
                            }
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            f_count = f_count * 10 + c - '0';
                            readnext = true;
                            break;

                        case 'h':
                            f_half++;
                            readnext = true;
                            break;

                        case 'l':
                            f_long++;
                            readnext = true;
                            break;

                        case 'L':
                            f_long = 2;
                            readnext = true;
                            break;

                        case 'c':
                            ok = write_uchar (stream, va_arg (ap, unsigned char));
                            break;

                        case 'd':
                        case 'i':
                            if (f_half > 1)
                                svalue = va_arg(ap, signed char);
                            else
                            if (f_half == 1)
                                svalue = va_arg(ap, signed short);
                            else
                            if (f_long == 1)
                                svalue = va_arg(ap, signed long);
                            else
                            if (f_long > 1)
                                svalue = va_arg(ap, signed long long);
                            else
                                svalue = va_arg(ap, signed int);
                            ok = write_decimal (stream, svalue, flags, f_count);
                            break;

                        case 'u':
                            if (f_half > 1)
                                uvalue = va_arg(ap, unsigned char);
                            else
                            if (f_half == 1)
                                uvalue = va_arg(ap, unsigned short);
                            else
                            if (f_long == 1)
                                uvalue = va_arg(ap, unsigned long);
                            else
                            if (f_long > 1)
                                uvalue = va_arg(ap, unsigned long long);
                            else
                                uvalue = va_arg(ap, unsigned int);
                            ok = write_udecimal (stream, uvalue, flags, f_count);
                            break;

                        case 'x':
                        case 'X':
                            if (c == 'X')
                                flags |= FORMATFL_UPPERCASE;

                            if (f_half > 1)
                                uvalue = va_arg(ap, unsigned char);
                            else
                            if (f_half == 1)
                                uvalue = va_arg(ap, unsigned short);
                            else
                            if (f_long == 1)
                                uvalue = va_arg(ap, unsigned long);
                            else
                            if (f_long > 1)
                                uvalue = va_arg(ap, unsigned long long);
                            else
                                uvalue = va_arg(ap, unsigned int);
                            ok = write_hexadecimal (stream, uvalue, flags, f_count);
                            break;

                        case 's':
                            pvalue = va_arg(ap, char *);
                            ok = datastream_write (stream, pvalue, strlen (pvalue));
                            break;

                        case 0:
                            end = true;
                            break;

                        default:
                            break;
                    }
                    i++;
                    j = i;
                } while (ok && readnext);
                break;

            case 0:
                if (i - j)
                    ok = datastream_write (stream, format + j, i - j);
                end = true;
                break;

            default:
                i++;
        }
    }

    if (ok)
        ok = datastream_flush (stream);

    return ok;
}
