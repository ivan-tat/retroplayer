/* filldma.c -- functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/string.h"
#include "dos/ems.h"
#include "hw/sb/sbctl.h"
#include "main/posttab.h"
#include "main/s3mvars.h"
#include "main/fillvars.h"
#include "main/mixer.h"
#include "main/mixing.h"

// TODO: remove PUBLIC_CODE macros when done.

void __near fill_8bit(void *mixbuf, SNDDMABUF *outbuf)
{
    uint16_t wait, framesize, framelen, srcoff, dstoff;
    uint8_t (*buf)[1];

    /* check if we are allready in calculation routines
       if we are the PC is too slow -> how you wanna handle it ? */

    buf = outbuf->buf->data;
    framesize = outbuf->frameSize;
        // size to fill
    framelen = snddmabuf_get_count_from_offset(outbuf, framesize);
        // samples per channel to fill

    if (outbuf->flags & SNDDMABUFFL_LOCKED)
    {
        wait = 0xffff;
        while (wait && (outbuf->flags & SNDDMABUFFL_LOCKED))
        {
            wait--;
        }
        if (outbuf->flags & SNDDMABUFFL_LOCKED)
        {
            /* sorry your PC is to slow - maincode may ignore this flag */
            /* but it'll sound ugly :( */
            outbuf->flags |= SNDDMABUFFL_SLOW;

            /* simply fill the half with last correct mixed value */
            dstoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameActive);
            outbuf->frameActive = 1 - outbuf->frameActive;
            srcoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameActive);
            // FIXME: 8-bits stereo is two byte fill, not one
            memset(&(buf[dstoff]), *buf[srcoff + framesize - 1], framesize);
            return;
        }
    }
    /* for check if too slow set a variable (flag that we are allready in calc) */

    outbuf->flags |= SNDDMABUFFL_LOCKED;

    if (playState_songEnded) {
        // clear buffer
        memset(&(buf[snddmabuf_get_frame_offset(outbuf, outbuf->frameActive)]), 0, framesize);
        outbuf->frameActive = 1 - outbuf->frameActive;
    } else {
        // mix into the mixing buffer
        calcTick(mixbuf, framelen);

        outbuf->frameLast = (outbuf->frameLast + 1) & (outbuf->framesCount - 1);
        dstoff = snddmabuf_get_frame_offset(outbuf, outbuf->frameLast);

        amplify_16s(mixbuf, framesize);

        if (outbuf->flags & SNDDMABUFFL_LQ)
        {
            if (sb_get_channels() == 2)
                clip_16s_stereo_8u_stereo_lq(&(buf[dstoff << 1]), mixbuf, framesize);
            else
                clip_16s_mono_8u_mono_lq(&(buf[dstoff << 1]), mixbuf, framesize);
        }
        else
            clip_16s_8u(&(buf[dstoff]), mixbuf, framesize);
    }

    outbuf->flags &= ~SNDDMABUFFL_LOCKED;
}

void PUBLIC_CODE fill_DMAbuffer(void *mixbuf, SNDDMABUF *outbuf)
{
    if (sb_get_sample_bits() == 8)
    {
        do
        {
            fill_8bit(mixbuf, outbuf);
        } while (outbuf->frameLast != outbuf->frameActive);
    }
}
