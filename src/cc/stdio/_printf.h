/* _printf.h -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_STDIO_PRINTF_H
#define CC_STDIO_PRINTF_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"

#include "cc/stdio.h"

/* Data stream */

typedef enum
{
    DATASTREAM_TYPE_UNDEFINED = 0,
    DATASTREAM_TYPE_MEMORY    = 1,
    DATASTREAM_TYPE_STDOUT    = 2,
    DATASTREAM_TYPE_FILE      = 3
} DATASTREAMTYPE;

typedef void __far dataStreamFlushProc_t(const char *buf);

typedef struct dataStream_t
{
    DATASTREAMTYPE type;
    size_t limit;   /* only for memory stream */
    bool stop;      /* only for memory stream */
    uint8_t termSize;
    uint32_t term;  /* 1-4 bytes or far pointer to terminator if longer */
    char *buf;      /* buffer */
    size_t bufSize; /* buffer */
    size_t pos;     /* buffer */
    size_t written;
    void *output;   /* "FILE *stream" or "char *ptr" */
    dataStreamFlushProc_t *flush;   /* only for printf() */
};
typedef struct dataStream_t DATASTREAM;

void dataStreamFlush(DATASTREAM *self);
void dataStreamWrite(DATASTREAM *self, const void *ptr, size_t len);
void dataStreamInitMemory(DATASTREAM *self, void *ptr, size_t limit);
void dataStreamInitStdOut(DATASTREAM *self, char *buf, size_t size);
void dataStreamInitFile(DATASTREAM *self, FILE *stream, char *buf, size_t size);

void _dsprintf(DATASTREAM *stream, const char *format, va_list ap);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux dataStreamFlush "*";
#pragma aux dataStreamWrite "*";
#pragma aux dataStreamInitMemory "*";
#pragma aux dataStreamInitStdOut "*";
#pragma aux dataStreamInitFile "*";
#pragma aux _dsprintf "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_STDIO_PRINTF_H */
