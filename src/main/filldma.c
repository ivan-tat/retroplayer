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

void __near fill_frame(MIXBUF *mb, SNDDMABUF *outbuf)
{
    void *mixbuf;
    uint8_t f_bits;
    bool f_sign;
    uint8_t f_channels;
    uint8_t f_width;
    uint16_t wait, frame_size, frame_spc, frame_len, srcoff, dstoff;
    uint8_t (*buf)[1];
    union
    {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
    } fill_value;

    mixbuf = mb->buf;

    f_bits = get_sample_format_bits(&(outbuf->format));
    f_sign = is_sample_format_signed(&(outbuf->format));
    f_channels = get_sample_format_channels(&(outbuf->format));
    f_width = get_sample_format_width(&(outbuf->format));

    buf = outbuf->buf->data;
    frame_size = outbuf->frameSize;
    frame_spc = snddmabuf_get_count_from_offset(outbuf, frame_size);
    frame_len = frame_spc * f_channels;

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
            srcoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameActive) + frame_size;

            switch (f_width)
            {
            case 8:
                fill_value.u8 = *((uint8_t *)(buf[srcoff - 1]));
                fill_8(&(buf[dstoff]), fill_value.u8, frame_len);
                break;
            case 16:
                fill_value.u16 = *((uint16_t *)(buf[srcoff - 2]));
                fill_16(&(buf[dstoff]), fill_value.u16, frame_len);
                break;
            case 32:
                fill_value.u32 = *((uint32_t *)(buf[srcoff - 4]));
                fill_32(&(buf[dstoff]), fill_value.u32, frame_len);
                break;
            default:
                break;
            }
            return;
        }
    }

    outbuf->flags |= SNDDMABUFFL_LOCKED;

    sound_fill_buffer(mb, frame_spc);

    amplify_16s(mixbuf, frame_len); // (16 bits)

    outbuf->frameLast = (outbuf->frameLast + 1) & (outbuf->framesCount - 1);
    dstoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameLast);

    if (outbuf->flags & SNDDMABUFFL_LQ)
    {
        switch (f_bits)
        {
        case 8:
            if (get_sample_format_channels(&(outbuf->format)) == 2)
                clip_16s_stereo_8u_stereo_lq(&(buf[dstoff << 1]), mixbuf, frame_len);
            else
                clip_16s_mono_8u_mono_lq(&(buf[dstoff << 1]), mixbuf, frame_len);
            break;
        /*
        case 16:
            if (get_sample_format_channels(&(outbuf->format)) == 2)
                clip_16_stereo_16_stereo_lq(&(buf[dstoff << 1]), mixbuf, frame_len);
            else
                clip_16_mono_16_mono_lq(&(buf[dstoff << 1]), mixbuf, frame_len);
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
            clip_16s_8u(&(buf[dstoff]), mixbuf, frame_len);
            break;
        /*
        case 16:
            clip_16_16(&(buf[dstoff]), mixbuf, frame_len);
            break;
        */
        default:
            break;
        }
    }

    outbuf->flags &= ~SNDDMABUFFL_LOCKED;
}

void PUBLIC_CODE fill_DMAbuffer(MIXBUF *mb, SNDDMABUF *outbuf)
{
    while (outbuf->frameLast != outbuf->frameActive)
    {
        fill_frame(mb, outbuf);
    }
}
