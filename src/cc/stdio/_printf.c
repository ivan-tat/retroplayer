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

#include "cc/stdio.h"
#include "cc/stdio/_printf.h"

extern void PUBLIC_CODE pascal_write(const char *str);

#ifdef __WATCOMC__
#pragma aux pascal_write modify [ax bx cx dx si di es];
#endif

/*** Data stream ***/

void __far _writestdout(const char *str)
{
    pascal_write(str);
}

void __near _dataStream_init(DATASTREAM *self)
{
    memset(self, 0, sizeof(DATASTREAM));
}

void __near _dataStreamTerminate(DATASTREAM *self)
{
    if (self->buf)
        if (self->termSize)
            memcpy(self->buf + self->pos,
                (self->termSize <= 4) ? &self->term : (void *)self->term,
                self->termSize);
}

void dataStreamFlush(DATASTREAM *self)
{
    size_t size;

    if (self->pos)
    {
        size = self->pos;
        switch (self->type)
        {
            case DATASTREAM_TYPE_MEMORY:
                /* we do not use buffer here */
                break;

            case DATASTREAM_TYPE_STDOUT:
                _dataStreamTerminate(self);
                self->flush(self->buf);
                self->written += size;
                break;

            case DATASTREAM_TYPE_FILE:
                if (self->output)
                {
                    _dataStreamTerminate(self);
                    if (fwrite(self->buf, size, 1, (FILE *)self->output))
                        self->written += size;
                };
                break;

            default:
                break;
        };
        self->pos = 0;
    };
}

void dataStreamWrite(DATASTREAM *self, const void *ptr, size_t len)
{
    const char *input;
    size_t maxsize, left, __n;

    switch (self->type)
    {
        case DATASTREAM_TYPE_MEMORY:
            if (self->output)
            {
                if (self->stop)
                    break;
                if (self->limit)
                {
                    left = self->limit - self->written;
                    if (len > left)
                        len = left;
                };
                memcpy(MK_FP(FP_SEG(self->output), FP_OFF(self->output) + self->pos),
                    ptr, len);
                self->pos += len;
                self->written += len;
                if (self->limit)
                    self->stop = (self->written == self->limit);
            };
            break;

        case DATASTREAM_TYPE_STDOUT:
        case DATASTREAM_TYPE_FILE:
            input = ptr;
            maxsize = self->bufSize - self->termSize;
            do
            {
                left = maxsize - self->pos;
                if (left > 0)
                {
                    __n = len;
                    if (__n > left)
                        __n = left;
                    memcpy(&self->buf[self->pos], input, __n);
                    input += __n;
                    self->pos += __n;
                    len -= __n;
                };
                if (self->pos == maxsize)
                    dataStreamFlush(self);
            } while (len && !self->stop);
            break;

        default:
            break;
    };
}

void dataStreamInitMemory(DATASTREAM *self, void *ptr, size_t limit)
{
    _dataStream_init(self);
    self->type = DATASTREAM_TYPE_MEMORY;
    self->output = ptr;
    self->limit = limit;
}

void dataStreamInitStdOut(DATASTREAM *self, char *buf, size_t size)
{
    _dataStream_init(self);
    self->type = DATASTREAM_TYPE_STDOUT;
    self->termSize = 1;
    //self->term = 0;    /* already set */
    self->buf = buf;
    self->bufSize = size;
    self->flush = &_writestdout;
}

void dataStreamInitFile(DATASTREAM *self, FILE *stream, char *buf, size_t size)
{
    _dataStream_init(self);
    self->type = DATASTREAM_TYPE_FILE;
    self->buf = buf;
    self->bufSize = size;
    self->output = (void *)stream;
}

/*** _dsprintf() ***/

#define HEXDIGIT_LC(x) ((x) < 10 ? '0' + (x) : 'a' + (x) - 10)
#define HEXDIGIT_UC(x) ((x) < 10 ? '0' + (x) : 'A' + (x) - 10)

void __near write_uchar
(
    DATASTREAM *stream,
    const unsigned char __v
)
{
    dataStreamWrite(stream, &__v, 1);
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
    };

    dataStreamWrite(stream, p, len);
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

    dataStreamWrite(stream, p, len);
}

/* ------------------------------------------------------------------ */

void _dsprintf(DATASTREAM *stream, const char *format, va_list ap)
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
                    dataStreamWrite(stream, format + j, i - j);
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
                            dataStreamWrite(stream, "%", 1);
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
                            dataStreamWrite(stream, pvalue, strlen(pvalue));
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
                    dataStreamWrite(stream, format + j, i - j);
                end = true;
                break;

            default:
                i++;
        };
    };
    dataStreamFlush(stream);
}
