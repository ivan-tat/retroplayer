/* filldma.c -- functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

#include "..\dos\emstool.h"
#include "..\blaster\sbctl.h"
#include "posttab.h"
#include "s3mvars.h"
#include "fillvars.h"
#include "mixvars.h"
#include "mixing.h"

static bool errorsav = false;

void convert_8(void *outbuf, void *mixbuf, uint16_t count)
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

void LQconvert_8(void *outbuf, void *mixbuf, uint16_t count)
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

void fill_8bit(void)
{
    uint16_t wait, bufsize, buflen, srcoff, dstoff;
    uint8_t (*buf)[1];

    /* check if we are allready in calculation routines
       if we are the PC is too slow -> how you wanna handle it ? */

    buf = DMABuf;
    bufsize = DMABufFrameSize;
        // size to fill
    buflen = getCountFromDMABufOff(bufsize);
        // samples per channel to fill

    if (DMAFlags_JustInFill) {
        wait = 0xffff;
        while (wait && DMAFlags_JustInFill) {
            wait--;
        }
        if (DMAFlags_JustInFill) {
            /* sorry your PC is to slow - maincode may ignore this flag */
            /* but it'll sound ugly :( */
            DMAFlags_Slow = true;

            /* simply fill the half with last correct mixed value */
            dstoff = getDMABufFrameOff(DMABufFrameActive);
            DMABufFrameActive = 1 - DMABufFrameActive;
            srcoff = getDMABufFrameOff(DMABufFrameActive);
            // FIXME: 8-bits stereo is two byte fill, not one
            memset(&(buf[dstoff]), *buf[srcoff + bufsize - 1], bufsize);
            return;
        }
    }
    /* for check if too slow set a variable (flag that we are allready in calc) */

    DMAFlags_JustInFill = true;

    if (EndOfSong) {
        // clear DMA buffer
        memset(&(buf[getDMABufFrameOff(DMABufFrameActive)]), 0, bufsize);
        DMABufFrameActive = 1 - DMABufFrameActive;
    } else {
        // before calling mixing routines: save EMM mapping !
        if (UseEMS) {
            errorsav = true;
            if (EmsSaveMap(SavHandle)) errorsav = false;
        }

        // mix into the mixing buffer
        calcTick(mixBuf, buflen);

        // now restore EMM mapping:
        if (UseEMS) {
            if (! errorsav) EmsRestoreMap(SavHandle);
        }

        DMABufFrameLast = (DMABufFrameLast + 1) & (DMABufFramesCount - 1);
        dstoff = getDMABufFrameOff(DMABufFrameLast);

        if (playOption_LowQuality)
            LQconvert_8(&(buf[dstoff << 1]), mixBuf, bufsize);
        else
            convert_8(&(buf[dstoff]), mixBuf, bufsize);
    }

    DMAFlags_JustInFill = false;
}

void PUBLIC_CODE fill_DMAbuffer(void)
{
    if (! sdev_mode_16bit) {
        do {
            fill_8bit();
        } while (DMABufFrameLast != DMABufFrameActive);
    }
}
