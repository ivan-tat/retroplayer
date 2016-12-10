/* mixing.c -- mixing routines.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

#include "..\dos\emstool.h"
#include "..\blaster\sbctl.h"
#include "s3mtypes.h"
#include "mixtypes.h"
#include "s3mvars.h"
#include "effvars.h"
#include "mixvars.h"
#include "fillvars.h"
#include "voltab.h"
#include "effects.h"
#include "mixer.h"
#include "mixer_.h"
#include "readnote.h"

/* Returns the length of mixing buffer in sample units:
 * SamplesPerChannel * ChannelsCount */
#define getMixBufLength() (_16bit ? DMARealBufSize[1] >> 1 : DMARealBufSize[1])

/* Returns the size of mixing buffer in bytes:
 * SamplesPerChannel * ChannelsCount */
#define getMixBufSize() (_16bit ? DMARealBufSize[1] : DMARealBufSize[1] << 1)

void *PUBLIC_CODE mapSampleData(uint16_t seg, uint16_t len)
{
    unsigned int logPage;
    unsigned char physPage, count;
    if (isSampleDataInEM(seg)) {
        logPage = getSampleDataLogPageInEM(seg);
        physPage = 0;
        count = (unsigned int)(((unsigned long)len + 0x3fff) >> 1) >> 13;
        while (count--) {
            if (! EmsMap(SmpEMSHandle, logPage++, physPage++))
                return MK_FP(0, 0);
        };
        return MK_FP(FrameSEG[0], 0);
    } else {
        return MK_FP(seg, 0);
    };
}

uint16_t PUBLIC_CODE getMixBufOffFromCount(uint16_t count)
{
    unsigned int bufOff = count;
    if (stereo) bufOff <<= 1;
    return bufOff << 1; /* int16_t (*TickBuffer)[]*/
};

uint16_t PUBLIC_CODE getCountFromMixBufOff(uint16_t bufOff)
{
    unsigned int count = bufOff >> 1;   /* int16_t (*TickBuffer)[]*/
    if (stereo) count >>= 1;
    return count;
};

void __near newTick(void)
{
    mixTickSamplesPerChannelLeft = mixTickSamplesPerChannel;
    if (CurTick <= 1) {
        if (PatternDelay != 0) {
            /* or pattern delay done ... */
            PatternDelay--;
            if (PatternDelay != 0) CurLine--;
        };
        readnewnotes();
    } else {
        CurTick--;
    };
}

void __near calcChannel(struct channel_t *chnInfo, bool callEffects, uint16_t count, void *outBuf)
{
    struct playSampleInfo_t smpInfo;
    unsigned int smpPos;

    if (chnInfo->bChannelType == 0 || chnInfo->bChannelType > 2) return;
    if (callEffects) {
        /* do effects for this channel: */
        if (CurTick != CurSpeed) chn_effTick(chnInfo);
    };
    /* check if mixing: */
    if (chnInfo->bEnabled == 0) return;

    smpInfo.dData = mapSampleData(chnInfo->wSmpSeg, chnInfo->wSmpLoopEnd);
    if (! smpInfo.dData) return; /* skip channel if EMS driver does not work correct */

    smpInfo.dPos = chnInfo->dSmpPos;
    smpInfo.dStep = chnInfo->dSmpStep;
    smpPos = chnInfo->dSmpPos >> 16;

    /* first check for correct position inside sample */
    if (smpPos < chnInfo->wSmpLoopEnd) {
        if (stereo)
            _MixSampleStereo8(outBuf, &smpInfo, FP_SEG(*volumetableptr), chnInfo->bSmpVol, count);
        else
            _MixSampleMono8(outBuf, &smpInfo, FP_SEG(*volumetableptr), chnInfo->bSmpVol, count);
        smpPos = smpInfo.dPos >> 16;
    };
    if (smpPos >= chnInfo->wSmpLoopEnd) {
        if (chnInfo->bSmpFlags & SMPFLAG_LOOP == 0) {
            chnInfo->bEnabled = 0;
        } else {
            while (smpPos >= chnInfo->wSmpLoopEnd) {
                smpPos -= chnInfo->wSmpLoopEnd;
                smpPos += chnInfo->wSmpLoopStart;
            };
        };
    };
    chnInfo->dSmpPos = (chnInfo->dSmpPos & 0xffff) + ((unsigned long)smpPos << 16);
}

void PUBLIC_CODE calcTick(void)
{
    uint16_t bufOff;
    uint16_t count;   /* N of samples per channel (mono/left/right) to calculate */
    uint8_t curChannel;
    bool callEffects;
    struct channel_t *chnInfo;

    /* first fill mixing buffer with zero */
    memset(TickBuffer, 0, getMixBufSize());

    bufOff = 0;

    if (mixTickSamplesPerChannelLeft) {
        callEffects = false;
    } else {
        callEffects = true;
        newTick();
    };

    while (! EndOfSong) {
        count = getCountFromMixBufOff(getMixBufSize() - bufOff);
        if (count > mixTickSamplesPerChannelLeft) count = mixTickSamplesPerChannelLeft;
            /* finish that tick and loop to fill the whole mixing buffer */

        if (count == 0) break;

        for (curChannel = 0; curChannel < UsedChannels; curChannel++) {
            chnInfo = &(Channel[curChannel]);
            calcChannel(chnInfo, callEffects, count,
                MK_FP(FP_SEG(TickBuffer), FP_OFF(TickBuffer) + (bufOff << 0) +
                (stereo && chnInfo->bChannelType == 1 ? 0 : 2)));
        };

        mixTickSamplesPerChannelLeft -= count;
        bufOff += getMixBufOffFromCount(count);

        if (bufOff < getMixBufSize()) {
            callEffects = true;
            newTick();
        } else {
            break;
        };
    };
};
