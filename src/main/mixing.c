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
#include "main/musmod.h"
#include "main/readnote.h"

#include "main/mixing.h"

void __near song_new_tick (MUSMOD *track, PLAYSTATE *ps, MIXCHNLIST *channels, MIXBUF *mb)
{
    ps->tick_samples_per_channel_left = ps->tick_samples_per_channel;

    if (ps->tick <= 1)
    {
        if (ps->patdelay_count)
        {
            ps->patdelay_count--;
            if (ps->patdelay_count)
                ps->row--;
        }
        readnewnotes (track, ps, channels);
    }
    else
    {
        ps->tick--;
    }
}

void __near song_play_channel (MUSMOD *track, PLAYSTATE *ps, MIXCHN *chn, bool callEffects, MIXBUF *mb, uint16_t count, uint16_t bufOff)
{
    void *outBuf;
    MIXCHNTYPE chtype;
    struct playSampleInfo_t smpInfo;
    unsigned int smpPos;
    MUSINS *ins;
    PCMSMP *smp;
    uint8_t Vol, IV, SV, GV, final_volume;

    outBuf = MK_FP(FP_SEG(mb->buf), FP_OFF(mb->buf) + bufOff);
    chtype = mixchn_get_type(chn);

    if (chtype != MIXCHNTYPE_PCM)
        return;

    if (callEffects)
    {
        /* do effects for this channel: */
        if (ps->tick != ps->speed)
            chn_effTick (track, ps, chn);
    }

    if (!mixchn_is_playing(chn))
        return;

    smpInfo.dPos = chn->dSmpPos;
    smpInfo.dStep = mixchn_get_sample_step(chn);
    smpPos = chn->dSmpPos >> 16;

    /* first check for correct position inside sample */
    if (smpPos < chn->wSmpLoopEnd)
    {
        ins = mixchn_get_instrument (chn);
        smp = mixchn_get_sample (chn);
        Vol = mixchn_get_note_volume (chn); // = 0..64  (6 bits)
        IV = musins_get_volume (ins);       // = 0..128 (7 bits)
        SV = pcmsmp_get_volume (smp);       // = 0..64  (6 bits)
        GV = ps->global_volume;             // = 0..64  (6 bits)
        if (Vol | IV | SV | GV)
        {
            // final_volume = 0..64 (6 bits)
            final_volume = ((uint32_t) Vol * IV * SV * GV) >> (6 + 7 + 6 + 6 - 6);
            if (final_volume > 64)
                final_volume = 64;
        }
        else
            final_volume = 0;

        if (mixchn_is_mixing(chn) && final_volume)
        {
            final_volume--; // fit it in 6 bits, 1..64 -> 0..63
            if (pcmsmp_is_EM_data (smp))
                smpInfo.dData = pcmsmp_map_EM_data (smp);
            else
                smpInfo.dData = pcmsmp_get_data (smp);

            if (!smpInfo.dData)
                return;

            if (mb->channels == 2)
                _MixSampleStereo8(outBuf, &smpInfo, FP_SEG(*volumetableptr), final_volume, count);
            else
                _MixSampleMono8(outBuf, &smpInfo, FP_SEG(*volumetableptr), final_volume, count);
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

void song_play (MUSMOD *track, PLAYSTATE *ps, MIXCHNLIST *channels, MIXBUF *mb, uint16_t len)
{
    void *outBuf;
    uint16_t bufSize;
    uint16_t bufOff;
    uint16_t count;   /* samples per channel to calculate */
    uint8_t i;
    bool callEffects;
    MIXCHN *chn;

    outBuf = mb->buf;
    bufSize = mixbuf_get_offset_from_count(mb, len);

    if (!(ps->flags & PLAYSTATEFL_END))
    {
        bufOff = 0;

        if (ps->tick_samples_per_channel_left)
        {
            callEffects = false;
        }
        else
        {
            callEffects = true;
            song_new_tick (track, ps, channels, mb);
        }

        while (!(ps->flags & PLAYSTATEFL_END))
        {
            count = mixbuf_get_count_from_offset(mb, bufSize - bufOff);
            if (count > ps->tick_samples_per_channel_left)
                count = ps->tick_samples_per_channel_left;
                /* finish that tick and loop to fill the whole mixing buffer */

            if (!count)
                break;

            for (i = 0; i < mixchnl_get_count (channels); i++)
            {
                chn = mixchnl_get (channels, i);
                if (mixchn_is_enabled(chn))
                    song_play_channel (track, ps, chn, callEffects, mb, count,
                        bufOff + ((mixbuf_get_channels (mb) == 2) && (mixchn_get_pan (chn) == MIXCHNPAN_RIGHT) ? sizeof(int32_t) : 0));   // NOTE: mixbuf is 32 bits
            }

            ps->tick_samples_per_channel_left -= count;
            bufOff += mixbuf_get_offset_from_count(mb, count);

            if (bufOff < bufSize)
            {
                callEffects = true;
                song_new_tick (track, ps, channels, mb);
            }
            else
                break;
        }
    }
}
