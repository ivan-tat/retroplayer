/* mixing.c -- mixing routines.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// TODO: remove PUBLIC_CODE macros when done.

#include "pascal/pascal.h"
#include "cc/i86.h"
#include "dos/ems.h"
#include "main/s3mtypes.h"
#include "main/mixtypes.h"
#include "main/s3mvars.h"
#include "main/effvars.h"
#include "main/mixvars.h"
#include "main/fillvars.h"
#include "main/voltab.h"
#include "main/effects.h"
#include "main/mixer.h"
#include "main/mixer_.h"
#include "main/readnote.h"

void *PUBLIC_CODE mapSampleData(uint16_t seg, uint16_t len)
{
    unsigned int logPage;
    unsigned char physPage, count;
    if (isSampleDataInEM(seg)) {
        logPage = getSampleDataLogPageInEM(seg);
        physPage = 0;
        count = (unsigned int)(((unsigned long)len + 0x3fff) >> 1) >> 13;
        while (count--) {
            if (! emsMap(SmpEMSHandle, logPage++, physPage++))
                return MK_FP(0, 0);
        };
        return emsFramePtr;
    } else {
        return MK_FP(seg, 0);
    };
}

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
        if (mixChannels == 2)
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

void PUBLIC_CODE calcTick(void *outBuf, uint16_t len)
{
    uint16_t bufSize;
    uint16_t bufOff;
    uint16_t count;   /* N of samples per channel (mono/left/right) to calculate */
    uint8_t ch;
    bool callEffects;
    struct channel_t *chnInfo;

    /* clear mixing buffer */
    bufSize = getMixBufOffFromCount(len);
    memset(outBuf, 0, bufSize);

    bufOff = 0;

    if (mixTickSamplesPerChannelLeft) {
        callEffects = false;
    } else {
        callEffects = true;
        newTick();
    };

    while (! EndOfSong) {
        count = getCountFromMixBufOff(bufSize - bufOff);
        if (count > mixTickSamplesPerChannelLeft) count = mixTickSamplesPerChannelLeft;
            /* finish that tick and loop to fill the whole mixing buffer */

        if (count == 0) break;

        for (ch = 0; ch < UsedChannels; ch++) {
            chnInfo = &(Channel[ch]);
            calcChannel(chnInfo, callEffects, count,
                MK_FP(FP_SEG(outBuf), FP_OFF(outBuf) + bufOff +
                (mixChannels == 2 && chnInfo->bChannelType == 2 ? 2 : 0)));
        };

        mixTickSamplesPerChannelLeft -= count;
        bufOff += getMixBufOffFromCount(count);

        if (bufOff < bufSize) {
            callEffects = true;
            newTick();
        } else {
            break;
        };
    };
};
