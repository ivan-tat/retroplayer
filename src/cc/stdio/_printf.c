/* _printf.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "dstream.h"

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

bool __far _system_flush_file (DATASTREAM *self)
{
    if (self->output)
        return (fwrite (self->buf, self->pos, 1, (FILE *) self->output)) == 1;
    else
        return false;
}

bool __far _system_flush_stdout (DATASTREAM *self)
{
    self->buf[self->pos] = 0;
    pascal_write (self->buf);
    return true;
}

/*** _printf () ***/

#define HEXDIGIT_LC(x) ((x) < 10 ? '0' + (x) : 'a' + (x) - 10)
#define HEXDIGIT_UC(x) ((x) < 10 ? '0' + (x) : 'A' + (x) - 10)

void __near write_uchar
(
    DATASTREAM *stream,
    const unsigned char __v
)
{
    datastream_write (stream, &__v, 1);
}

void __near write_udecimal
(
    DATASTREAM *stream,
    const unsigned long __v,
    const bool          sign,
    const bool          leadingzeroes,
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
            *p = leadingzeroes ? '0' : ' ';
        len++;
    } while (_value || _count);
    if (sign)
    {
        *--p = '-';
        len++;
    }

    datastream_write (stream, p, len);
}

/* ------------------------------------------------------------------ */

void __near write_decimal
(
    DATASTREAM *stream,
    const signed long   __v,
    const bool          leadingzeroes,
    const unsigned char __n
)
{
    unsigned long _value;
    bool sign;

    sign = __v < 0;
    if (sign)
        _value = -__v;
    else
        _value = __v;

    write_udecimal(stream, _value, sign, leadingzeroes, __n);
}

/* ------------------------------------------------------------------ */

void __near write_hexadecimal
(
    DATASTREAM *stream,
    const unsigned long __v,
    const bool          uppercase,
    const bool          leadingzeroes,
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
            *p = uppercase ? HEXDIGIT_UC(_value & 15) : HEXDIGIT_LC(_value & 15);
            _value >>= 4;
        }
        else
            *p = leadingzeroes ? '0' : ' ';
        len++;
    } while (_value || _count);

    datastream_write (stream, p, len);
}

/* ------------------------------------------------------------------ */

void __far _printf (DATASTREAM *stream, const char *format, va_list ap)
{
    int i, j;
    bool end;
    bool readnext;
    char c;
    /* Flag characters */
    bool          f_leadingzeroes;  /* 0 */
        /* Unsupported: "#", "-", " ", "+", "'", "I" */
    /* Field width */
    unsigned char f_count;          /* number */
        /* Unsupported: "." */
    /* Length modifiers */
    unsigned char f_half;           /* h, hh */
    unsigned char f_long;           /* l, ll, L */
        /* Unsupported: j, z, t */
    /* Value modifiers (internal) */
    bool          f_uppercase;
    /* Values (internal)*/
    signed long   svalue;
    unsigned long uvalue;
    void         *pvalue;

    i = 0;
    j = 0;
    end = false;
    while (!end)
    {
        switch (c = format[i])
        {
            case '%':
                if (i - j > 0)
                    datastream_write (stream, format + j, i - j);
                i++;
                f_leadingzeroes = false;
                f_count = 0;
                f_half = 0;
                f_long = 0;
                f_uppercase = false;

                do
                {
                    readnext = false;
                    switch (c = format[i])
                    {
                        case '%':
                            datastream_write (stream, "%", 1);
                            break;

                        case '0':
                            if (f_count == 0)
                            {
                                f_leadingzeroes = true;
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
                            write_uchar(stream, va_arg(ap, unsigned char));
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
                            write_decimal(stream, svalue, f_leadingzeroes, f_count);
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
                            write_udecimal(stream, uvalue, false, f_leadingzeroes, f_count);
                            break;

                        case 'x':
                        case 'X':
                            f_uppercase = (c == 'X');

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
                            write_hexadecimal(stream, uvalue, f_uppercase, f_leadingzeroes, f_count);
                            break;

                        case 's':
                            pvalue = va_arg(ap, char *);
                            datastream_write (stream, pvalue, strlen(pvalue));
                            break;

                        case '\0':
                            end = true;
                            break;

                        default:
                            break;
                    }
                    i++;
                    j = i;
                } while (readnext);
                break;

            case '\0':
                if (i - j)
                    datastream_write (stream, format + j, i - j);
                end = true;
                break;

            default:
                i++;
        }
    }
    datastream_flush (stream);
}
