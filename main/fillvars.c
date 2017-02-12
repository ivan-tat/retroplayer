/* fillvars.c -- variables for functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#endif

/* TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done */

#include "..\pascal\pascal.h"
#include "..\ow\stdio.h"
#include "..\dos\dosproc.h"
#include "..\hw\dma.h"
#include "..\blaster\sbctl.h"
#include "s3mvars.h"
#include "fillvars.h"

uint16_t PUBLIC_CODE sndDMABufGetFrameOff(SNDDMABUF *buf, uint8_t index)
{
    return index < buf->framesCount ? index * buf->frameSize : 0;
}

uint16_t PUBLIC_CODE sndDMABufGetOffFromCount(SNDDMABUF *buf, uint16_t count)
{
    unsigned int bufOff = count;
    if (get_sample_format_bits(&(buf->format)) == 16) bufOff <<= 1;
    if (get_sample_format_channels(&(buf->format)) == 2) bufOff <<= 1;
    if (playOption_LowQuality) bufOff <<= 1;
    return bufOff;
}

uint16_t PUBLIC_CODE sndDMABufGetCountFromOff(SNDDMABUF *buf, uint16_t bufOff)
{
    unsigned int count = bufOff;
    if (get_sample_format_bits(&(buf->format)) == 16) count >>= 1;
    if (get_sample_format_channels(&(buf->format)) == 2) count >>= 1;
    if (playOption_LowQuality) count >>= 1;
    return count;
}

bool PUBLIC_CODE sndDMABufAlloc(SNDDMABUF *buf, uint32_t dmaSize)
{
    return dmaBufAlloc(buf->buf, dmaSize);
}

void PUBLIC_CODE sndDMABufFree(SNDDMABUF *buf)
{
    if (buf->buf)
        dmaBufFree(buf->buf);
    clear_sample_format(&(buf->format));
    buf->frameSize = 0;
    buf->framesCount = 0;
    buf->frameLast = 0;
    buf->frameActive = 0;
    buf->flags_locked = false;
    buf->flags_Slow = false;
}

void PUBLIC_CODE sndDMABufInit(SNDDMABUF *buf)
{
    buf->buf = dmaBuf_new();
    clear_sample_format(&(buf->format));
    buf->frameSize = 0;
    buf->framesCount = 0;
    buf->frameLast = 0;
    buf->frameActive = 0;
    buf->flags_locked = false;
    buf->flags_Slow = false;
}

void PUBLIC_CODE sndDMABufDone(SNDDMABUF *buf)
{
    if (buf->buf)
    {
        sndDMABufFree(buf);
        dmaBufDone(buf->buf);
        dmaBuf_delete(&(buf->buf));
    };
}
