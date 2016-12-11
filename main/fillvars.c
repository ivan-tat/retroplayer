/* fillvars.c -- variables for functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#endif

/* TODO: remove EXTERN_LINK, PUBLIC_DATA and PUBLIC_CODE macros when done */

#include "..\pascal\pascal.h"

#include "..\blaster\sbctl.h"
#include "..\dos\dosproc.h"
#include "..\watcomc\printf.h"

/* DMA buffer */

EXTERN_LINK void    *PUBLIC_DATA DMABufUnaligned;
EXTERN_LINK void    *PUBLIC_DATA DMABuf;
EXTERN_LINK uint32_t PUBLIC_DATA DMABufSize;
EXTERN_LINK uint16_t PUBLIC_DATA DMABufFrameSize;
EXTERN_LINK uint8_t  PUBLIC_DATA DMABufFramesCount;
EXTERN_LINK uint8_t  PUBLIC_DATA DMABufFrameLast;
EXTERN_LINK uint8_t  PUBLIC_DATA DMABufFrameActive;
EXTERN_LINK bool     PUBLIC_DATA DMAFlags_JustInFill;
EXTERN_LINK bool     PUBLIC_DATA DMAFlags_Slow;

/* player */

EXTERN_LINK uint8_t PUBLIC_DATA playOption_FPS;
EXTERN_LINK bool    PUBLIC_DATA playOption_LowQuality;

/* EMM */

EXTERN_LINK uint16_t PUBLIC_DATA SavHandle;

#define getLinearAddress(p) (((uint32_t)FP_SEG(p) << 4) + FP_OFF(p))

uint16_t PUBLIC_CODE getDMABufFrameOff(uint8_t index)
{
    return index < DMABufFramesCount ? index * DMABufFrameSize : 0;
}

uint16_t PUBLIC_CODE getDMABufOffFromCount(uint16_t count)
{
    unsigned int bufOff = count;
    if (sdev_mode_16bit) bufOff <<= 1;
    if (sdev_mode_stereo) bufOff <<= 1;
    if (playOption_LowQuality) bufOff <<= 1;
    return bufOff;
};

uint16_t PUBLIC_CODE getCountFromDMABufOff(uint16_t bufOff)
{
    unsigned int count = bufOff;
    if (sdev_mode_16bit) count >>= 1;
    if (sdev_mode_stereo) count >>= 1;
    if (playOption_LowQuality) count >>= 1;
    return count;
};

void PUBLIC_CODE initDMABuf(void)
{
    DMABufUnaligned = (void *)0;
    DMABuf = (void *)0;
    DMABufSize = 0;
    DMABufFrameSize = 0;
    DMABufFramesCount = 0;
}

bool PUBLIC_CODE allocDMABuf(uint32_t dmaSize)
{
    uint32_t bufStart, bufEnd, bufSize, dmaStart, dmaEnd;

    /* 64 KiB max. limit (for 8-bits channel) */
    dmaSize = dmaSize > 0x10000 ? 0x10000 : ((dmaSize + 15) & 0x1fff0);

    bufSize = dmaSize << 1;
    if (! getdosmem(&DMABufUnaligned, bufSize)) return false;

    bufStart = getLinearAddress(DMABufUnaligned);
    bufEnd = bufStart + bufSize - 1;

    #ifdef DEBUG
    printf("[info] Allocated %uli bytes of DOS memory for DMA buffer at 0x%05ulX-0x%05ulX\r\n",
        (uint32_t)bufSize, (uint32_t)bufStart, (uint32_t)bufEnd);
    #endif

    dmaStart = bufStart;
    dmaEnd = dmaStart + dmaSize - 1;

    if (((uint32_t)dmaStart & 0xf0000) != ((uint32_t)dmaEnd & 0xf0000)) {
        dmaStart = (bufStart & 0xf0000) + 0x10000;
        dmaEnd = dmaStart + dmaSize - 1;
    }

    DMABufSize = dmaSize;
    DMABuf = MK_FP(dmaStart >> 4, 0);
    #ifdef DEBUG
    printf("[info] Using %uli bytes for DMA buffer at 0x%05ulX-0x%05ulX\r\n",
        (uint32_t)DMABufSize, (uint32_t)dmaStart, (uint32_t)dmaEnd);
    #endif

    if (dmaEnd < bufEnd) {
        #ifdef DEBUG
        printf("[info] Freeing unused trailing %uli bytes of allocated DMA buffer\r\n",
            (uint32_t)(bufEnd - dmaEnd));
        #endif
        bufSize = dmaEnd - bufStart + 1;
        setsize(DMABufUnaligned, bufSize);
    }
    memset(DMABuf, 0, DMABufSize);

    return true;
}

void PUBLIC_CODE freeDMABuf(void)
{
    freedosmem(DMABufUnaligned);
    DMABuf = (void *)0;
    DMABufSize = 0;
    DMABufFrameSize = 0;
    DMABufFramesCount = 0;
}

void PUBLIC_CODE doneDMABuf(void)
{
    if (DMABufUnaligned) freeDMABuf();
}
