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
#include "main/mixvars.h"
#include "main/mixing.h"

// TODO: remove PUBLIC_CODE macros when done.

static bool errorsav = false;

void __near convert_8(void *outbuf, void *mixbuf, uint16_t count)
{
    uint16_t *src;
    uint8_t *dst;

    src = (uint16_t *)mixbuf;
    dst = (uint8_t *)outbuf;

    do {
        *dst = post8bit[*src+2048];
        src++;
        dst++;
        count--;
    } while (count);
}

void __near LQconvert_8(void *outbuf, void *mixbuf, uint16_t count)
{
    uint16_t (*src)[1];
    uint8_t (*dst)[1];
    uint8_t samp[2];

    src = (uint16_t *)mixbuf;
    dst = (uint8_t *)outbuf;

    if (sdev_mode_stereo) {
        count >>= 1;
        do {
            samp[0] = post8bit[*src[0]+2048];
            samp[1] = post8bit[*src[1]+2048];
            *dst[0] = samp[0];
            *dst[1] = samp[1];
            *dst[2] = samp[0];
            *dst[3] = samp[1];
            src += 2;
            dst += 4;
            count--;
        } while (count);
    } else {
        do {
            samp[0] = post8bit[*src[0]+2048];
            *dst[0] = samp[0];
            *dst[1] = samp[0];
            src++;
            dst += 2;
            count--;
        } while (count);
    }
}

void __near fill_8bit(void *mixbuf, SNDDMABUF *outbuf)
{
    uint16_t wait, framesize, framelen, srcoff, dstoff;
    uint8_t (*buf)[1];

    /* check if we are allready in calculation routines
       if we are the PC is too slow -> how you wanna handle it ? */

    buf = outbuf->buf->data;
    framesize = outbuf->frameSize;
        // size to fill
    framelen = sndDMABufGetCountFromOff(&sndDMABuf, framesize);
        // samples per channel to fill

    if (sndDMABuf.flags_locked) {
        wait = 0xffff;
        while (wait && sndDMABuf.flags_locked) {
            wait--;
        }
        if (sndDMABuf.flags_locked) {
            /* sorry your PC is to slow - maincode may ignore this flag */
            /* but it'll sound ugly :( */
            sndDMABuf.flags_Slow = true;

            /* simply fill the half with last correct mixed value */
            dstoff = sndDMABufGetFrameOff(&sndDMABuf, sndDMABuf.frameActive);
            sndDMABuf.frameActive = 1 - sndDMABuf.frameActive;
            srcoff = sndDMABufGetFrameOff(&sndDMABuf, sndDMABuf.frameActive);
            // FIXME: 8-bits stereo is two byte fill, not one
            memset(&(buf[dstoff]), *buf[srcoff + framesize - 1], framesize);
            return;
        }
    }
    /* for check if too slow set a variable (flag that we are allready in calc) */

    sndDMABuf.flags_locked = true;

    if (playState_songEnded) {
        // clear buffer
        memset(&(buf[sndDMABufGetFrameOff(&sndDMABuf, sndDMABuf.frameActive)]), 0, framesize);
        sndDMABuf.frameActive = 1 - sndDMABuf.frameActive;
    } else {
        // before calling mixing routines: save EMM mapping !
        if (UseEMS) {
            errorsav = true;
            if (emsSaveMap(SavHandle)) errorsav = false;
        }

        // mix into the mixing buffer
        calcTick(mixbuf, framelen);

        // now restore EMM mapping:
        if (UseEMS) {
            if (! errorsav) emsRestoreMap(SavHandle);
        }

        sndDMABuf.frameLast = (sndDMABuf.frameLast + 1) & (sndDMABuf.framesCount - 1);
        dstoff = sndDMABufGetFrameOff(&sndDMABuf, sndDMABuf.frameLast);

        if (playOption_LowQuality)
            LQconvert_8(&(buf[dstoff << 1]), mixbuf, framesize);
        else
            convert_8(&(buf[dstoff]), mixbuf, framesize);
    }

    sndDMABuf.flags_locked = false;
}

void PUBLIC_CODE fill_DMAbuffer(void *mixbuf, SNDDMABUF *outbuf)
{
    if (! sdev_mode_16bit) {
        do {
            fill_8bit(mixbuf, outbuf);
        } while (outbuf->frameLast != outbuf->frameActive);
    }
}
