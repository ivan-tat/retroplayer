/* fillvars.c -- variables for functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$main$fillvars$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/stdio.h"
#include "cc/string.h"
#include "debug.h"
#include "common.h"
#include "hw/dma.h"
#include "hw/sb/sbctl.h"
#include "main/fillvars.h"

void __far snddmabuf_init (SNDDMABUF *self)
{
    if (self)
    {
        self->flags = 0;
        self->buf = NULL;
        clear_sample_format(&(self->format));
        self->frameSize = 0;
        self->framesCount = 0;
        self->frameLast = 0;
        self->frameActive = 0;
    }
}

bool __far snddmabuf_alloc (SNDDMABUF *self, uint32_t dmaSize)
{
    DEBUG_BEGIN ();

    if (self)
    {
        if (!self->buf)
            self->buf = _new(DMABUF);

        if (!self->buf)
        {
            DEBUG_ERR ("Failed to initialize DMA buffer object.");
            return false;
        }

        dmaBuf_init(self->buf);

        if (dmaBuf_alloc(self->buf, dmaSize))
        {
            DEBUG_SUCCESS ();
            return true;
        }
        else
        {
            DEBUG_ERR ("Failed to allocate DMA buffer.");
            return false;
        }
    }
    else
    {
        DEBUG_ERR ("Self is NULL.");
        return false;
    }
}

uint16_t __near _snddmabuf_get_frame_offset(SNDDMABUF *self, uint8_t index)
{
    return index < self->framesCount ? index * self->frameSize : 0;
}

uint16_t __far snddmabuf_get_frame_offset (SNDDMABUF *self, uint8_t index)
{
    if (self)
        return _snddmabuf_get_frame_offset(self, index);
    else
        return 0;
}

void *__far snddmabuf_get_frame (SNDDMABUF *self, uint8_t index)
{
    void *data;

    if (self && self->buf)
    {
        data = self->buf->data;
        if (data)
            return MK_FP(FP_SEG(data), FP_OFF(data) + _snddmabuf_get_frame_offset(self, index));
    }

    return NULL;
}

uint16_t __far snddmabuf_get_offset_from_count (SNDDMABUF *self, uint16_t count)
{
    uint16_t bufOff;

    if (self)
    {
        bufOff = count;

        if (get_sample_format_bits(&(self->format)) == 16)
            bufOff <<= 1;

        if (get_sample_format_channels(&(self->format)) == 2)
            bufOff <<= 1;

        if (self->flags & SNDDMABUFFL_LQ)
            bufOff <<= 1;
    }
    else
        bufOff = 0;

    return bufOff;
}

uint16_t __far snddmabuf_get_count_from_offset (SNDDMABUF *self, uint16_t bufOff)
{
    uint16_t count;

    if (self)
    {
        count = bufOff;

        if (get_sample_format_bits(&(self->format)) == 16)
            count >>= 1;

        if (get_sample_format_channels(&(self->format)) == 2)
            count >>= 1;

        if (self->flags & SNDDMABUFFL_LQ)
            count >>= 1;
    }
    else
        count = 0;

    return count;
}

void __far snddmabuf_free (SNDDMABUF *self)
{
    if (self)
        if (self->buf)
        {
            dmaBuf_free(self->buf);
            _delete(self->buf);
        }
}
