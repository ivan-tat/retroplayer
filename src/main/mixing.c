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
#include "main/musmod.h"
#include "main/musmodps.h"
#include "main/mixer.h"
#include "main/voltab.h"
#include "main/effvars.h"
#include "main/effects.h"
#include "main/readnote.h"

#include "main/mixing.h"

void __near song_new_tick (PLAYSTATE *ps)
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
        readnewnotes (ps);
    }
    else
    {
        ps->tick--;
    }
}

void __near song_play_channel (PLAYSTATE *ps, MIXCHN *chn, bool callEffects, MIXER *mixer, uint16_t count, uint16_t bufOff)
{
    SMPBUF *sb;
    void *smpbuf;
    MIXBUF *mb;
    void *outBuf;
    MIXCHNTYPE chtype;
    struct playSampleInfo_t smpInfo;
    unsigned int smpPos;
    MUSINS *ins;
    PCMSMP *smp;
    uint8_t NV, IV, SV, GV, final_volume;

    mb = mixer_get_mixbuf (mixer);
    outBuf = (void *)((uint8_t *)mb->buf + bufOff);
    chtype = mixchn_get_type(chn);

    if (chtype != MIXCHNTYPE_PCM)
        return;

    if (callEffects)
    {
        /* do effects for this channel: */
        if (ps->tick != ps->speed)
            chn_effTick (ps, chn);
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
        NV = mixchn_get_note_volume (chn);  // = 0..64  (6 bits)
        IV = musins_get_volume (ins);       // = 0..128 (7 bits)
        SV = pcmsmp_get_volume (smp);       // = 0..64  (6 bits)
        GV = ps->global_volume;             // = 0..64  (6 bits)
        if (NV && IV && SV && GV)
        {
            // final_volume = 0..64 (6 bits)
            final_volume = ((uint32_t) NV * IV * SV * GV) >> (6 + 7 + 6 + 6 - 6);
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

            sb = mixer_get_smpbuf (mixer);
            smpbuf = smpbuf_get (sb);

            switch (mixer_get_quality (mixer))
            {
            case MIXQ_NEAREST:
                if (pcmsmp_get_bits (smp) == 16)
                    _play_sample_nearest_16 (smpbuf, &smpInfo, count);
                else
                    _play_sample_nearest_8 (smpbuf, &smpInfo, count);

                if (mb->channels == 2)
                    _mix_sample2 (outBuf, smpbuf, FP_SEG(*volumetableptr), final_volume, count);
                else
                    _mix_sample (outBuf, smpbuf, FP_SEG(*volumetableptr), final_volume, count);
            default:
                // fastest
                if (pcmsmp_get_bits (smp) == 16)
                {
                    if (mb->channels == 2)
                        _MixSampleStereo16 (outBuf, &smpInfo, FP_SEG(*volumetableptr), final_volume, count);
                    else
                        _MixSampleMono16 (outBuf, &smpInfo, FP_SEG(*volumetableptr), final_volume, count);
                }
                else
                {
                    if (mb->channels == 2)
                        _MixSampleStereo8 (outBuf, &smpInfo, FP_SEG(*volumetableptr), final_volume, count);
                    else
                        _MixSampleMono8 (outBuf, &smpInfo, FP_SEG(*volumetableptr), final_volume, count);
                }
                break;
            }
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

void song_play (PLAYSTATE *ps, MIXER *mixer, uint16_t len)
{
    MIXCHNLIST *channels;
    MIXBUF *mb;
    void *outBuf;
    uint16_t bufSize;
    uint16_t bufOff;
    uint16_t count;   /* samples per channel to calculate */
    uint8_t i;
    bool callEffects;
    MIXCHN *chn;

    mb = mixer_get_mixbuf (mixer);
    outBuf = mixbuf_get (mb);
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
            song_new_tick (ps);
        }

        channels = ps->channels;
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
                    song_play_channel (ps, chn, callEffects, mixer, count,
                        bufOff + ((mixbuf_get_channels (mb) == 2) && (mixchn_get_pan (chn) == MIXCHNPAN_RIGHT) ? sizeof(int32_t) : 0));   // NOTE: mixbuf is 32 bits
            }

            ps->tick_samples_per_channel_left -= count;
            bufOff += mixbuf_get_offset_from_count(mb, count);

            if (bufOff < bufSize)
            {
                callEffects = true;
                song_new_tick (ps);
            }
            else
                break;
        }
    }
}
