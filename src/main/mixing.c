/* mixing.c -- mixing routines.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
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

// TODO: remove PUBLIC_CODE macros when done.

void __near newTick(void)
{
    mixTickSamplesPerChannelLeft = mixTickSamplesPerChannel;
    if (playState_tick <= 1) {
        if (playState_patDelayCount != 0) {
            /* or pattern delay done ... */
            playState_patDelayCount--;
            if (playState_patDelayCount != 0) playState_row--;
        };
        readnewnotes();
    } else {
        playState_tick--;
    };
}

void __near calcChannel(MIXCHN *chnInfo, bool callEffects, uint16_t count, void *outBuf)
{
    uint8_t chtype;
    struct playSampleInfo_t smpInfo;
    unsigned int smpPos;
    MUSINS *ins;

    chtype = mixchn_get_type(chnInfo);
    if (chtype == 0 || chtype > 2) return;
    if (callEffects) {
        /* do effects for this channel: */
        if (playState_tick != playState_speed) chn_effTick(chnInfo);
    };

    if (!mixchn_is_playing(chnInfo))
        return;

    smpInfo.dPos = chnInfo->dSmpPos;
    smpInfo.dStep = mixchn_get_sample_step(chnInfo);
    smpPos = chnInfo->dSmpPos >> 16;

    /* first check for correct position inside sample */
    if (smpPos < chnInfo->wSmpLoopEnd)
    {
        if (mixchn_is_mixing(chnInfo))
        {
            ins = mixchn_get_instrument(chnInfo);
            if (musins_is_EM_data(ins))
                smpInfo.dData = musins_map_EM_data(ins);
            else
                smpInfo.dData = musins_get_data(ins);

            if (!smpInfo.dData)
                return;

            if (mixChannels == 2)
                _MixSampleStereo8(outBuf, &smpInfo, FP_SEG(*volumetableptr), mixchn_get_sample_volume(chnInfo), count);
            else
                _MixSampleMono8(outBuf, &smpInfo, FP_SEG(*volumetableptr), mixchn_get_sample_volume(chnInfo), count);
        }
        else
        {
            // TODO: silent play
        }
        smpPos = smpInfo.dPos >> 16;
    }
    if (smpPos >= chnInfo->wSmpLoopEnd) {
        if (chnInfo->bSmpFlags & SMPFLAG_LOOP == 0) {
            mixchn_set_playing(chnInfo, false);
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
    MIXCHN *chnInfo;

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

    while (! playState_songEnded) {
        count = getCountFromMixBufOff(bufSize - bufOff);
        if (count > mixTickSamplesPerChannelLeft) count = mixTickSamplesPerChannelLeft;
            /* finish that tick and loop to fill the whole mixing buffer */

        if (count == 0) break;

        for (ch = 0; ch < UsedChannels; ch++)
        {
            chnInfo = &(Channel[ch]);
            if (mixchn_is_enabled(chnInfo))
                calcChannel(chnInfo, callEffects, count,
                    MK_FP(FP_SEG(outBuf), FP_OFF(outBuf) + bufOff +
                    (mixChannels == 2 && mixchn_get_type(chnInfo) == 2 ? 2 : 0)));
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
