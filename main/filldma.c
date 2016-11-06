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
#include "mixing.h"

static bool errorsav = false;

void mixroutines(void)
{
    __asm "push ds";    // FIXME
    calc_tick();
    __asm "pop ds";     // FIXME
}

void convert_8(void *outbuf, void *mixbuf, uint16_t count)
{
    uint16_t *src;
    uint8_t *dst;

    src = (uint16_t *)mixbuf;
    dst = (uint8_t *)outbuf;

    do {
        *dst = post8bit[*src];
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

    if (stereo) {
        count >>= 1;
        do {
            samp[0] = post8bit[*src[0]];
            samp[1] = post8bit[*src[1]];
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
            samp[0] = post8bit[*src[0]];
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
    uint16_t wait, bufsize, srcoff, dstoff;
    uint8_t (*buf)[1];

    /* check if we are allready in calculation routines
       if we are the PC is too slow -> how you wanna handle it ? */

    buf = DMAbuffer;
    bufsize = DMARealBufSize[1]; // we have to calc. DMARealBufSize bytes

    if (JustInFill) {
        wait = 0xffff;
        while (wait && JustInFill) {
            wait--;
        }
        if (JustInFill) {
            /* sorry your PC is to slow - maincode may ignore this flag */
            /* but it'll sound ugly :( */
            TooSlow = true;

            /* simply fill the half with last correct mixed value */
            dstoff = DMARealBufSize[DMAHalf];
            DMAHalf = 1-DMAHalf;
            srcoff = DMARealBufSize[DMAHalf];
            // FIXME: 8-bits stereo is two byte fill, not one
            memset(&(buf[dstoff]), *buf[srcoff + bufsize - 1], bufsize);
            return;
        }
    }
    /* for check if too slow set a variable (flag that we are allready in calc) */

    JustInFill = true;

    if (EndOfSong) {
        // clear DMAbuffer
        memset(&(buf[DMARealBufSize[DMAHalf]]), 0, bufsize);
        DMAHalf = 1 - DMAHalf;
    } else {
        // before calling mixroutines: save EMM mapping !
        if (UseEMS) {
            errorsav = true;
            if (EmsSaveMap(SavHandle)) errorsav = false;
        }

        mixroutines(); // calc 'DMARealBufSize' bytes into the TickBuffer

        // now restore EMM mapping:
        if (UseEMS) {
            if (! errorsav) EmsRestoreMap(SavHandle);
        }

        LastReady = (LastReady + 1) & (NumBuffers - 1);
        dstoff = DMARealBufSize[LastReady];

        if (LQMode)
            LQconvert_8(&(buf[dstoff << 1]), TickBuffer, bufsize);
        else
            convert_8(&(buf[dstoff]), TickBuffer, bufsize);
    }

    JustInFill = false;
}

void PUBLIC_CODE fill_DMAbuffer(void)
{
    if (! _16bit) {
        do {
            fill_8bit();
        } while (LastReady != DMAHalf);
    }
}
