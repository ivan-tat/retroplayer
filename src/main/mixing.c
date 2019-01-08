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
#include "main/s3mvars.h"
#include "main/effvars.h"
#include "main/fillvars.h"
#include "main/voltab.h"
#include "main/effects.h"
#include "main/mixer.h"
#include "main/readnote.h"

#include "main/mixing.h"

void __near song_new_tick(MIXBUF *mb)
{
    playState_tick_samples_per_channel_left = playState_tick_samples_per_channel;

    if (playState_tick <= 1)
    {
        if (playState_patDelayCount)
        {
            playState_patDelayCount--;
            if (playState_patDelayCount)
                playState_row--;
        }
        readnewnotes();
    }
    else
    {
        playState_tick--;
    }
}

void __near song_play_channel(MIXCHN *chn, bool callEffects, MIXBUF *mb, uint16_t count, uint16_t bufOff)
{
    void *outBuf;
    MIXCHNTYPE chtype;
    struct playSampleInfo_t smpInfo;
    unsigned int smpPos;
    PCMSMP *smp;
    uint8_t vol;

    outBuf = MK_FP(FP_SEG(mb->buf), FP_OFF(mb->buf) + bufOff);
    chtype = mixchn_get_type(chn);

    if (chtype != MIXCHNTYPE_PCM)
        return;

    if (callEffects)
    {
        /* do effects for this channel: */
        if (playState_tick != playState_speed)
            chn_effTick(chn);
    }

    if (!mixchn_is_playing(chn))
        return;

    smpInfo.dPos = chn->dSmpPos;
    smpInfo.dStep = mixchn_get_sample_step(chn);
    smpPos = chn->dSmpPos >> 16;

    /* first check for correct position inside sample */
    if (smpPos < chn->wSmpLoopEnd)
    {
        vol = mixchn_get_note_volume (chn);
        if (mixchn_is_mixing(chn) && vol)
        {
            vol--;
            smp = mixchn_get_sample (chn);
            if (pcmsmp_is_EM_data (smp))
                smpInfo.dData = pcmsmp_map_EM_data (smp);
            else
                smpInfo.dData = pcmsmp_get_data (smp);

            if (!smpInfo.dData)
                return;

            if (mb->channels == 2)
                _MixSampleStereo8(outBuf, &smpInfo, FP_SEG(*volumetableptr), vol, count);
            else
                _MixSampleMono8(outBuf, &smpInfo, FP_SEG(*volumetableptr), vol, count);
        }
        else
        {
            // TODO: silent play
        }
        smpPos = smpInfo.dPos >> 16;
    }
    if (smpPos >= chn->wSmpLoopEnd)
    {
        if (chn->bSmpFlags & MIXSMPFL_LOOP)
        {
            while (smpPos >= chn->wSmpLoopEnd)
            {
                smpPos -= chn->wSmpLoopEnd;
                smpPos += chn->wSmpLoopStart;
            }
        }
        else
        {
            mixchn_set_playing(chn, false);
        }
    }
    chn->dSmpPos = (chn->dSmpPos & 0xffff) + ((unsigned long)smpPos << 16);
}

void __near song_play(MIXBUF *mb, uint16_t len)
{
    void *outBuf;
    uint16_t bufSize;
    uint16_t bufOff;
    uint16_t count;   /* samples per channel to calculate */
    uint8_t i;
    bool callEffects;
    MIXCHNLIST *channels;
    MIXCHN *chn;

    outBuf = mb->buf;
    bufSize = mixbuf_get_offset_from_count(mb, len);
    channels = mod_Channels;

    if (!playState_songEnded)
    {
        bufOff = 0;

        if (playState_tick_samples_per_channel_left)
        {
            callEffects = false;
        }
        else
        {
            callEffects = true;
            song_new_tick(mb);
        }

        while (! playState_songEnded)
        {
            count = mixbuf_get_count_from_offset(mb, bufSize - bufOff);
            if (count > playState_tick_samples_per_channel_left)
                count = playState_tick_samples_per_channel_left;
                /* finish that tick and loop to fill the whole mixing buffer */

            if (!count)
                break;

            for (i = 0; i < mixchnl_get_count (channels); i++)
            {
                chn = mixchnl_get (channels, i);
                if (mixchn_is_enabled(chn))
                    song_play_channel(chn, callEffects, mb, count,
                        bufOff + ((mixbuf_get_channels (mb) == 2) && (mixchn_get_pan (chn) == MIXCHNPAN_RIGHT) ? sizeof(int32_t) : 0));   // NOTE: mixbuf is 32 bits
            }

            playState_tick_samples_per_channel_left -= count;
            bufOff += mixbuf_get_offset_from_count(mb, count);

            if (bufOff < bufSize)
            {
                callEffects = true;
                song_new_tick(mb);
            }
            else
                break;
        }
    }
}

void sound_fill_buffer(MIXBUF *mb, uint16_t len)
{
    uint16_t size;

    /* clear mixing buffer */
    size = mixbuf_get_offset_from_count(mb, len);
    memset(mb->buf, 0, size);

    song_play(mb, len);
}
