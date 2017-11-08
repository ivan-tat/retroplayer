/* fillvars.c -- variables for functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

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
#include "main/s3mvars.h"
#include "main/fillvars.h"

/* TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done */

void PUBLIC_CODE snddmabuf_init(SNDDMABUF *self)
{
    if (self)
    {
        self->buf = NULL;
        clear_sample_format(&(self->format));
        self->frameSize = 0;
        self->framesCount = 0;
        self->frameLast = 0;
        self->frameActive = 0;
        self->flags_locked = false;
        self->flags_Slow = false;
    }
}

bool PUBLIC_CODE snddmabuf_alloc(SNDDMABUF *self, uint32_t dmaSize)
{
    DEBUG_BEGIN("snddmabuf_alloc");

    if (self)
    {
        if (!self->buf)
            self->buf = _new(DMABUF);

        if (!self->buf)
        {
            DEBUG_FAIL("snddmabuf_alloc", "Failed to initialize DMA buffer object.");
            return false;
        }

        if (dmaBuf_alloc(self->buf, dmaSize))
        {
            DEBUG_SUCCESS("snddmabuf_alloc");
            return true;
        }
        else
        {
            DEBUG_FAIL("snddmabuf_alloc", "Failed to allocate DMA buffer.");
            return false;
        }
    }
    else
    {
        DEBUG_FAIL("snddmabuf_alloc", "Self is NULL.");
        return false;
    }
}

uint16_t __near _snddmabuf_get_frame_offset(SNDDMABUF *self, uint8_t index)
{
    return index < self->framesCount ? index * self->frameSize : 0;
}

uint16_t PUBLIC_CODE snddmabuf_get_frame_offset(SNDDMABUF *self, uint8_t index)
{
    if (self)
        return _snddmabuf_get_frame_offset(self, index);
    else
        return 0;
}

void *PUBLIC_CODE snddmabuf_get_frame(SNDDMABUF *self, uint8_t index)
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

uint16_t PUBLIC_CODE snddmabuf_get_offset_from_count(SNDDMABUF *self, uint16_t count)
{
    uint16_t bufOff;

    if (self)
    {
        bufOff = count;

        if (get_sample_format_bits(&(self->format)) == 16)
            bufOff <<= 1;

        if (get_sample_format_channels(&(self->format)) == 2)
            bufOff <<= 1;

        if (playOption_LowQuality)
            bufOff <<= 1;
    }
    else
        bufOff = 0;

    return bufOff;
}

uint16_t PUBLIC_CODE snddmabuf_get_count_from_offset(SNDDMABUF *self, uint16_t bufOff)
{
    uint16_t count;

    if (self)
    {
        count = bufOff;

        if (get_sample_format_bits(&(self->format)) == 16)
            count >>= 1;

        if (get_sample_format_channels(&(self->format)) == 2)
            count >>= 1;

        if (playOption_LowQuality)
            count >>= 1;
    }
    else
        count = 0;

    return count;
}

void PUBLIC_CODE snddmabuf_free(SNDDMABUF *self)
{
    if (self)
        if (self->buf)
        {
            dmaBuf_free(self->buf);
            _delete(self->buf);
        }
}
