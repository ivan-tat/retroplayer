/* dstream.c -- simple data stream handling library.

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

/*** Data stream ***/

void __far datastream_init (DATASTREAM *self, DATASTREAMFLAGS flags, bool (*__far _flush) (DATASTREAM *self))
{
    memset(self, 0, sizeof (DATASTREAM));
    _datastream_set_flags (self, flags);
    _datastream_set_flush (self, _flush);
}

bool __far datastream_write (DATASTREAM *self, const void *ptr, size_t len)
{
    const char *input;
    char *output;
    size_t maxsize, left, __n;
    bool ok;

    if ((self->flags & DSFLAG_STOP) == 0)
    {
        ok = true;

        if (self->flags & DSFLAG_BUFFER)
        {
            /* buffered */
            input = ptr;
            maxsize = self->buf_size;
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
                }

                if (self->pos == maxsize)
                    ok = datastream_flush (self);

            } while (len && ((self->flags & DSFLAG_STOP) == 0));
        }
        else
            /* direct */
            if (self->output)
            {
                if (self->limit)
                {
                    left = self->limit - self->written;
                    if (len > left)
                        len = left;
                }
                output = (char *) self->output + self->pos;
                memcpy (output, ptr, len);
                self->pos += len;
                self->written += len;
                if (self->limit)
                    if (self->written == self->limit)
                        self->flags |= DSFLAG_STOP;
            }
    }
    else
        ok = false;

    return ok;
}

bool __far datastream_flush (DATASTREAM *self)
{
    size_t size;
    bool ok;

    ok = true;

    if (self->pos)
    {
        /* we do use flush() in buffered mode only */
        if (self->flags & DSFLAG_BUFFER)
        {
            size = self->pos;
            ok = self->m_flush (self);
            if (ok)
                self->written += size;
            else
                self->flags |= DSFLAG_STOP | DSFLAG_ERROR;
        }
        /* zero position */
        self->pos = 0;
    }

    return ok;
}
