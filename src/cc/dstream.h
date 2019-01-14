/* dstream.h -- declarations for dstream.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef DSTREAM_H
#define DSTREAM_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "cc/i86.h"
#include "cc/string.h"

/* Data stream */

typedef uint16_t datastream_flags_t;
typedef datastream_flags_t DATASTREAMFLAGS;

#define DSFLAG_DIRECT   0
#define DSFLAG_BUFFER   (1 << 0)
#define DSFLAG_STOP     (1 << 1)
#define DSFLAG_ERROR    (1 << 2)

typedef struct datastream_t DATASTREAM;
typedef struct datastream_t
{
    DATASTREAMFLAGS flags;
    bool (*__far m_flush) (DATASTREAM *self);
    size_t limit;       /* direct mode only */
    size_t buf_size;    /* buffer */
    char *buf;          /* buffer */
    size_t pos;         /* buffer */
    size_t written;
    void *output;       /* parameter to user flush() method */
};

#define _datastream_get_flags(o)            (o)->flags
#define _datastream_set_flags(o, v)         _datastream_get_flags (o) = (v)
#define _datastream_get_flush(o)            (o)->m_flush
#define _datastream_set_flush(o, v)         _datastream_get_flush (o) = (v)
#define _datastream_get_limit(o)            (o)->limit
#define _datastream_set_limit(o, v)         _datastream_get_limit (o) = (v)
#define _datastream_get_buf_size(o)         (o)->buf_size
#define _datastream_set_buf_size(o, v)      _datastream_get_buf_size (o) = (v)
#define _datastream_get_buf_ptr(o)          (o)->buf
#define _datastream_set_buf_ptr(o, v)       _datastream_get_buf_ptr (o) = (v)
#define _datastream_get_output(o)           (o)->output
#define _datastream_set_output(o, v)        _datastream_get_output (o) = (v)

#define datastream_get_flags(o)         _datastream_get_flags (o)
#define datastream_set_flags(o, v)      _datastream_set_flags (o, v)
#define datastream_set_limit(o, v)      _datastream_set_limit (o, v)
#define datastream_set_buf_size(o, v)   _datastream_set_buf_size (o, v)
#define datastream_set_buf_ptr(o, v)    _datastream_set_buf_ptr (o, v)
#define datastream_set_output(o, v)     _datastream_set_output (o, v)

void __far datastream_init (DATASTREAM *self, DATASTREAMFLAGS flags, bool (*__far _flush) (DATASTREAM *self));
bool __far datastream_write (DATASTREAM *self, const void *ptr, size_t len);
bool __far datastream_flush (DATASTREAM *self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux datastream_init "*";
#pragma aux datastream_write "*";
#pragma aux datastream_flush "*";

#endif  /* __WATCOMC__ */

#endif  /* DSTREAM_H */
