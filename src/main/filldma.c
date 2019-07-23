/* filldma.c -- functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$main$filldma$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/string.h"
#include "main/musmodps.h"
#include "main/mixer.h"
#include "main/mixing.h"
#include "main/fillvars.h"
#include "main/posttab.h"
#include "main/filldma.h"

#if DEBUG_WRITE_OUTPUT_STREAM == 1

void __far DEBUG_open_output_streams (void)
{
    _debug_stream[0] = fopen ("_STREAM0", "wb");
    _debug_stream[1] = fopen ("_STREAM1", "wb");
}

void __far DEBUG_close_output_streams (void)
{
    if (_debug_stream[0])
        fclose (_debug_stream[0]);
    if (_debug_stream[1])
        fclose (_debug_stream[1]);
}

#endif  /* DEBUG_WRITE_OUTPUT_STREAM */

typedef void clip_proc_t(void *, int32_t *, uint16_t);

static clip_proc_t *clip_procs[] =  // NOTE: mixbuf is 32 bits
{
    &clip_s32_u8,
    &clip_s32_s8,
    &clip_s32_u16,
    &clip_s32_s16,
    &clip_s32_u8_lq,
    &clip_s32_s8_lq,
    &clip_s32_u16_lq,
    &clip_s32_s16_lq,
    &clip_s32_u8_lq_stereo,
    &clip_s32_s8_lq_stereo,
    &clip_s32_u16_lq_stereo,
    &clip_s32_s16_lq_stereo
};

#define CLIP_8 0
#define CLIP_16 2
#define CLIP_8_LQ 4
#define CLIP_16_LQ 6
#define CLIP_8_LQ_STEREO 8
#define CLIP_16_LQ_STEREO 10

void __near clear_frame (SNDDMABUF *outbuf)
{
    uint8_t (*data)[1];
    uint16_t frame_size, frame_spc, frame_len, srcoff, dstoff;
    uint8_t framePrev;
    union
    {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
    } fill_value;

    data = outbuf->buf->data;
    frame_size = outbuf->frameSize;
    frame_spc = snddmabuf_get_count_from_offset (outbuf, frame_size);
    frame_len = frame_spc * get_sample_format_channels (& (outbuf->format));

    /* simply fill the half with last correct mixed value */
    dstoff = snddmabuf_get_frame_offset (outbuf, outbuf->frameActive);
    framePrev = (outbuf->frameActive - 1) & (outbuf->framesCount - 1);
    srcoff = snddmabuf_get_frame_offset (outbuf, framePrev) + frame_size;

    switch (get_sample_format_width (& (outbuf->format)))
    {
    case 8:
        fill_value.u8 = *((uint8_t *) (data [srcoff - 1]));
        fill_8 (&(data [dstoff]), fill_value.u8, frame_len);
        break;
    case 16:
        fill_value.u16 = *((uint16_t *) (data [srcoff - 2]));
        fill_16 (&(data [dstoff]), fill_value.u16, frame_len);
        break;
    case 32:
        fill_value.u32 = *((uint32_t *) (data [srcoff - 4]));
        fill_32 (& (data [dstoff]), fill_value.u32, frame_len);
        break;
    default:
        break;
    }
}

void __near fill_frame (PLAYSTATE *ps, MIXER *mixer, SNDDMABUF *outbuf)
{
    MIXBUF *mb;
    void *mixbuf;
    uint8_t f_bits;
    bool f_signed;
    uint8_t f_channels;
    uint8_t f_width;
    uint16_t frame_size, frame_spc, frame_len, srcoff, dstoff;
    uint8_t (*buf)[1];
    int8_t clip;

    mb = mixer_get_mixbuf (mixer);
    mixbuf = mixbuf_get (mb);

    f_bits = get_sample_format_bits(&(outbuf->format));
    f_signed = is_sample_format_signed(&(outbuf->format));
    f_channels = get_sample_format_channels(&(outbuf->format));
    f_width = get_sample_format_width(&(outbuf->format));

    frame_size = outbuf->frameSize;
    frame_spc = snddmabuf_get_count_from_offset(outbuf, frame_size);
    frame_len = frame_spc * f_channels;

    /* clear mixing buffer */
    memset (mixbuf, 0, mixbuf_get_offset_from_count (mb, frame_spc));

    song_play (ps, mixer, frame_spc);

    amplify_s32(mixbuf, frame_len); // NOTE: mixbuf is 32 bits

    #if DEBUG_WRITE_OUTPUT_STREAM == 1
    if (_debug_stream[0])
        fwrite (mixbuf, frame_len * sizeof (int32_t), 1, _debug_stream[0]);
    #endif  /* DEBUG_WRITE_OUTPUT_STREAM */

    dstoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameLast);

    clip = -1;

    if (outbuf->flags & SNDDMABUFFL_LQ)
    {
        //dstoff <<= 1;
        switch (f_channels)
        {
        case 1:
            switch (f_bits)
            {
            case 8:
                clip = CLIP_8_LQ;
                break;
            case 16:
                clip = CLIP_16_LQ;
                break;
            default:
                break;
            }
            break;
        case 2:
            switch (f_bits)
            {
            case 8:
                clip = CLIP_8_LQ_STEREO;
                break;
            case 16:
                clip = CLIP_16_LQ_STEREO;
                break;
            default:
                break;
            }
        default:
            break;
        }
    }
    else
    {
        switch (f_bits)
        {
        case 8:
            clip = CLIP_8;
            break;
        case 16:
            clip = CLIP_16;
            break;
        default:
            break;
        }
    }

    buf = outbuf->buf->data;

    if (clip >= 0)
    {
        if (f_signed)
            clip++;

        if (clip_procs[clip])
            clip_procs[clip](&(buf[dstoff]), mixbuf, frame_len);
    }

    #if DEBUG_WRITE_OUTPUT_STREAM == 1
    if (_debug_stream[1])
        fwrite (& (buf[dstoff]), frame_size, 1, _debug_stream[1]);
    #endif  /* DEBUG_WRITE_OUTPUT_STREAM */
}

void __far fill_DMAbuffer (PLAYSTATE *ps, MIXER *mixer, SNDDMABUF *outbuf)
{
    while (outbuf->frameLast != outbuf->frameActive)
    {
        // critical section
        _disable ();
        if (outbuf->flags & SNDDMABUFFL_LOCKED)
        {
            outbuf->flags |= SNDDMABUFFL_SLOW;
            _enable ();
        }
        else
        {
            outbuf->flags |= SNDDMABUFFL_LOCKED;
            outbuf->frameLast = (outbuf->frameLast + 1) & (outbuf->framesCount - 1);
            _enable ();

            fill_frame (ps, mixer, outbuf);

            // critical section
            _disable ();
            outbuf->flags &= ~SNDDMABUFFL_LOCKED;
            _enable ();
        }
    }
}
