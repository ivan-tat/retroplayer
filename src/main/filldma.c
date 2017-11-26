/* filldma.c -- functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/string.h"
#include "dos/ems.h"
#include "main/posttab.h"
#include "main/s3mvars.h"
#include "main/fillvars.h"
#include "main/mixer.h"
#include "main/mixing.h"

// TODO: remove PUBLIC_CODE macros when done.

void __near fill_8(void *dest, uint8_t value, uint16_t size)
{
    memset(dest, value, size);
}

void __near fill_16(void *dest, uint16_t value, uint16_t size)
{
    uint16_t *p;
    register uint16_t count;

    p = dest;
    count = size / 2;
    while (count)
    {
        *p = value;
        p++;
    }
}

void __near fill_32(void *dest, uint32_t value, uint16_t size)
{
    uint32_t *p;
    register uint16_t count;

    p = dest;
    count = size / 4;
    while (count)
    {
        *p = value;
        p++;
    }
}

void __near fill_frame(void *mixbuf, SNDDMABUF *outbuf)
{
    uint8_t f_bits;
    bool f_sign;
    uint8_t f_channels;
    uint8_t f_width;
    uint16_t wait, framesize, framelen, srcoff, dstoff;
    uint8_t (*buf)[1];
    union
    {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
    } fill_value;

    f_bits = get_sample_format_bits(&(outbuf->format));
    f_sign = is_sample_format_signed(&(outbuf->format));
    f_channels = get_sample_format_channels(&(outbuf->format));
    f_width = get_sample_format_width(&(outbuf->format));

    buf = outbuf->buf->data;
    framesize = outbuf->frameSize;
    framelen = snddmabuf_get_count_from_offset(outbuf, framesize);

    if (outbuf->flags & SNDDMABUFFL_LOCKED)
    {
        wait = 0xffff;
        while (wait && (outbuf->flags & SNDDMABUFFL_LOCKED))
        {
            wait--;
        }
        if (outbuf->flags & SNDDMABUFFL_LOCKED)
        {
            outbuf->flags |= SNDDMABUFFL_SLOW;

            /* simply fill the half with last correct mixed value */
            dstoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameActive);
            outbuf->frameActive = 1 - outbuf->frameActive;
            srcoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameActive) + framesize;

            switch (f_width)
            {
            case 8:
                fill_value.u8 = *((uint8_t *)(buf[srcoff - 1]));
                fill_8(&(buf[dstoff]), fill_value.u8, framesize);
                break;
            case 16:
                fill_value.u16 = *((uint16_t *)(buf[srcoff - 2]));
                fill_16(&(buf[dstoff]), fill_value.u16, framesize);
                break;
            case 32:
                fill_value.u32 = *((uint32_t *)(buf[srcoff - 4]));
                fill_32(&(buf[dstoff]), fill_value.u32, framesize);
                break;
            default:
                break;
            }
            return;
        }
    }

    outbuf->flags |= SNDDMABUFFL_LOCKED;

    if (playState_songEnded)
    {
        dstoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameActive);
        switch (f_bits)
        {
        case 8:
            switch (f_channels)
            {
            case 1:
                if (f_sign)
                    fill_value.u8 = 0;
                else
                    fill_value.u8 = 0x80;
                fill_8(&(buf[dstoff]), fill_value.u8, framesize);
                break;
            case 2:
                if (f_sign)
                    fill_value.u16 = 0;
                else
                    fill_value.u16 = 0x8080;
                fill_16(&(buf[dstoff]), fill_value.u16, framesize);
                break;
            default:
                break;
            }
            break;
        case 16:
            switch (f_channels)
            {
            case 1:
                if (f_sign)
                    fill_value.u16 = 0;
                else
                    fill_value.u16 = 0x8000;
                fill_16(&(buf[dstoff]), fill_value.u16, framesize);
                break;
            case 2:
                if (f_sign)
                    fill_value.u32 = 0;
                else
                    fill_value.u32 = 0x80008000;
                fill_32(&(buf[dstoff]), fill_value.u32, framesize);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        outbuf->frameActive = 1 - outbuf->frameActive;
    }
    else
    {
        // mix into the mixing buffer
        calcTick(mixbuf, framelen);

        outbuf->frameLast = (outbuf->frameLast + 1) & (outbuf->framesCount - 1);
        dstoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameLast);

        amplify_16s(mixbuf, framesize);

        if (outbuf->flags & SNDDMABUFFL_LQ)
        {
            switch (f_bits)
            {
            case 8:
                if (get_sample_format_channels(&(outbuf->format)) == 2)
                    clip_16s_stereo_8u_stereo_lq(&(buf[dstoff << 1]), mixbuf, framesize);
                else
                    clip_16s_mono_8u_mono_lq(&(buf[dstoff << 1]), mixbuf, framesize);
                break;
            /*
            case 16:
                if (get_sample_format_channels(&(outbuf->format)) == 2)
                    clip_16_stereo_16_stereo_lq(&(buf[dstoff << 1]), mixbuf, framesize);
                else
                    clip_16_mono_16_mono_lq(&(buf[dstoff << 1]), mixbuf, framesize);
                break;
            */
            default:
                break;
            }
        }
        else
        {
            switch (f_bits)
            {
            case 8:
                clip_16s_8u(&(buf[dstoff]), mixbuf, framesize);
                break;
            /*
            case 16:
                clip_16_16(&(buf[dstoff]), mixbuf, framesize);
                break;
            */
            default:
                break;
            }
        }
    }

    outbuf->flags &= ~SNDDMABUFFL_LOCKED;
}

void PUBLIC_CODE fill_DMAbuffer(void *mixbuf, SNDDMABUF *outbuf)
{
    while (outbuf->frameLast != outbuf->frameActive)
    {
        fill_frame(mixbuf, outbuf);
    }
}
