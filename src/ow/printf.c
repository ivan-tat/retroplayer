/* printf.c -- Simplified printf() function definition.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#endif

#include "pascal/pascal.h"
#include "ow/stdio.h"

extern void __near __pascal printstring(char *s);

#ifdef __WATCOMC__
#pragma aux printstring modify [ax bx cx dx si di];
#endif

#define HEXDIGITS_LC(x) ((x) < 10 ? '0' + (x) : 'a' + (x) - 10)
#define HEXDIGITS_UC(x) ((x) < 10 ? '0' + (x) : 'A' + (x) - 10)

/* ------------------------------------------------------------------ */

typedef struct dataStream_t {
    char        *buf;
    unsigned int size;
    unsigned int pos;
};
typedef struct dataStream_t DATASTREAM;

void __near textStreamFlush(DATASTREAM *obj)
{
    if (obj->pos)
    {
        obj->buf[obj->pos] = 0;
        printstring(obj->buf);
        obj->pos = 0;
    };
}

void __near textStreamWriteBlock(DATASTREAM *obj, char *s, unsigned int len)
{
    char *input;
    unsigned int left, count;
    input = s;
    do
    {
        left = obj->size - 1 - obj->pos;
        count = len <= left ? len : left;
        memcpy(&obj->buf[obj->pos], input, count);
        input += count;
        obj->pos += count;
        if (obj->pos == obj->size - 1)
            textStreamFlush(obj);
        len -= count;
    } while (len);
}

void __near textStreamWrite(DATASTREAM *obj, char *s)
{
    textStreamWriteBlock(obj, s, strlen(s));
}

void __near textStreamInit(DATASTREAM *obj, char *buf, const unsigned int size)
{
    obj->buf = buf;
    obj->size = size;
    obj->pos = 0;
}

void __near textStreamDone(DATASTREAM *obj)
{
    textStreamFlush(obj);
}

/* ------------------------------------------------------------------ */

void __near print_decimal (
    DATASTREAM *stream,
    const unsigned long value,
    const bool          sign,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    unsigned long _value;
    unsigned char _count;
    char s[10];
    char *p;
    int len;

    p = s + 10;
    len = 0;
    _value = value;
    _count = count <= 10 ? count : 10;
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
    };

    textStreamWriteBlock(stream, p, len);
}

/* ------------------------------------------------------------------ */

void __near print_decimal_signed(
    DATASTREAM *stream,
    const signed long   value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    unsigned long _value;
    bool sign;

    sign = value < 0;
    if (sign)
        _value = -value;
    else
        _value = value;

    print_decimal(stream, _value, sign, leadingzeroes, count);
}

/* ------------------------------------------------------------------ */

void __near print_hexadecimal(
    DATASTREAM *stream,
    const unsigned long value,
    const bool          uppercase,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    unsigned long _value;
    unsigned char _count;
    char s[8];
    char *p;
    int len;

    p = s + 8;
    len = 0;
    _value = value;
    _count = count <= 8 ? count : 8;
    do
    {
        if (_count)
            _count--;
        p--;
        if (_value || len == 0)
        {
            *p = uppercase ? HEXDIGITS_UC(_value & 15) : HEXDIGITS_LC(_value & 15);
            _value >>= 4;
        }
        else
            *p = leadingzeroes ? '0' : ' ';
        len++;
    } while (_value || _count);

    textStreamWriteBlock(stream, p, len);
}

/* ------------------------------------------------------------------ */

void __near print_uchar(
    DATASTREAM *stream,
    const unsigned char value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal(stream, value, false, leadingzeroes, count);
}

/* ------------------------------------------------------------------ */

void __near print_char(
    DATASTREAM *stream,
    const signed char   value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal_signed(stream, value, leadingzeroes, count);
}

/* ------------------------------------------------------------------ */

void __near print_uint(
    DATASTREAM *stream,
    const unsigned int  value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal(stream, value, false, leadingzeroes, count);
}

/* ------------------------------------------------------------------ */

void __near print_int(
    DATASTREAM *stream,
    const signed int    value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal_signed(stream, value, leadingzeroes, count);
}

/* ------------------------------------------------------------------ */

void __near print_ulong(
    DATASTREAM *stream,
    const unsigned long value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal(stream, value, false, leadingzeroes, count);
}

/* ------------------------------------------------------------------ */

void __near print_long(
    DATASTREAM *stream,
    const signed long   value,
    const bool          leadingzeroes,
    const unsigned char count
)
{
    print_decimal_signed(stream, value, leadingzeroes, count);
}

/* ------------------------------------------------------------------ */

void printf(char *format, ...)
{
    va_list parminfo;
    char buf[256];
    DATASTREAM stream;
    int i, j;
    bool end;
    bool readnext;
    bool          f_long;
    bool          f_unsigned;
    bool          f_leadingzeroes;
    bool          f_uppercase;
    unsigned char f_count;

    textStreamInit(&stream, &buf, 256);
    va_start(parminfo, format);
    i = 0;
    j = 0;
    end = false;
    while (!end)
    {
        switch (format[i])
        {
        case '%':
            if (i - j > 0)
            {
                //format[i] = '\0';
                textStreamWriteBlock(&stream, format + j, i - j);
                //format[i] = '%';
            };
            i++;
            f_long = false;
            f_unsigned = false;
            f_leadingzeroes = false;
            f_uppercase = false;
            f_count = 0;

            do
            {
                readnext = false;
                switch (format[i])
                {
                case '%':
                    textStreamWrite(&stream, "%");
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
                    f_count = f_count * 10 + format[i] - '0';
                    readnext = true;
                    break;
                case 'c':
                case 'C':
                    if (f_unsigned)
                        print_uchar(&stream, va_arg(parminfo, unsigned char), f_leadingzeroes, f_count);
                    else
                        print_char(&stream, va_arg(parminfo, signed char), f_leadingzeroes, f_count);
                    break;
                case 'd':
                case 'D':
                case 'i':
                case 'I':
                    if (f_long)
                    {
                        if (f_unsigned)
                            print_uint(&stream, va_arg(parminfo, unsigned long), f_leadingzeroes, f_count);
                        else
                            print_int(&stream, va_arg(parminfo, signed long), f_leadingzeroes, f_count);
                    }
                    else
                    {
                        if (f_unsigned)
                            print_uint(&stream, va_arg(parminfo, unsigned int), f_leadingzeroes, f_count);
                        else
                            print_int(&stream, va_arg(parminfo, signed int), f_leadingzeroes, f_count);
                    }
                    break;
                case 'h':
                case 'H':
                        if (f_unsigned)
                            print_uint(&stream, va_arg(parminfo, unsigned short), f_leadingzeroes, f_count);
                        else
                            print_int(&stream, va_arg(parminfo, signed short), f_leadingzeroes, f_count);
                case 'l':
                case 'L':
                    f_long = true;
                    readnext = true;
                    break;
                case 's':
                case 'S':
                    textStreamWrite(&stream, va_arg(parminfo, char *));
                    break;
                case 'u':
                case 'U':
                    f_unsigned = true;
                    readnext = true;
                    break;
                case 'x':
                    if (f_long)
                        print_hexadecimal(&stream, va_arg(parminfo, unsigned long), false, f_leadingzeroes, f_count);
                    else
                        print_hexadecimal(&stream, va_arg(parminfo, unsigned int), false, f_leadingzeroes, f_count);
                    break;
                case 'X':
                    if (f_long)
                        print_hexadecimal(&stream, va_arg(parminfo, unsigned long), true, f_leadingzeroes, f_count);
                    else
                        print_hexadecimal(&stream, va_arg(parminfo, unsigned int), true, f_leadingzeroes, f_count);
                    break;
                case '\0':
                    end = true;
                    break;
                }
                i++;
                j = i;
            } while (readnext);
            break;

        case '\0':
            if (i - j)
                textStreamWrite(&stream, format + j);
            end = true;
            break;

        default:
            i++;
        };
    };
    textStreamDone(&stream);

    va_end(parminfo);
}
