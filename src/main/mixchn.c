/* mixchn.c -- mixing channel handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>

#include "main/effvars.h"
#include "main/effects.h"
#include "main/s3mvars.h"
#include "main/musmod.h"
#include "main/mixer.h"

#include "main/mixchn.h"

MIXCHNFLAGS __far __mixchn_set_flags (MIXCHNFLAGS _flags, MIXCHNFLAGS _mask, MIXCHNFLAGS _set, bool raise)
{
    if (raise)
        return (_flags & _mask) | _set;
    else
        return _flags & _mask;
}

void __far mixchn_init (MIXCHN *self)
{
    if (self)
        memset (self, 0, sizeof (MIXCHN));
}

void __far mixchn_set_flags (MIXCHN *self, MIXCHNFLAGS value)
{
    self->bChannelFlags = value;
}

MIXCHNFLAGS __far mixchn_get_flags (MIXCHN *self)
{
    return self->bChannelFlags;
}

void __far mixchn_set_enabled (MIXCHN *self, bool value)
{
    if (value)
        self->bChannelFlags |= MIXCHNFL_ENABLED;
    else
        self->bChannelFlags &= ~MIXCHNFL_ENABLED;
}

bool __far mixchn_is_enabled (MIXCHN *self)
{
    return (self->bChannelFlags & MIXCHNFL_ENABLED) != 0;
}

void __far mixchn_set_playing (MIXCHN *self, bool value)
{
    if (value)
        self->bChannelFlags |= MIXCHNFL_PLAYING;
    else
        self->bChannelFlags &= ~MIXCHNFL_PLAYING;
}

bool __far mixchn_is_playing (MIXCHN *self)
{
    return (self->bChannelFlags & MIXCHNFL_PLAYING) != 0;
}

void __far mixchn_set_mixing (MIXCHN *self, bool value)
{
    if (value)
        self->bChannelFlags |= MIXCHNFL_MIXING;
    else
        self->bChannelFlags &= ~MIXCHNFL_MIXING;
}

bool __far mixchn_is_mixing (MIXCHN *self)
{
    return (self->bChannelFlags & MIXCHNFL_MIXING) != 0;
}

void __far mixchn_set_pan (MIXCHN *self, MIXCHNPAN value)
{
    self->pan = value;
}

MIXCHNPAN __far mixchn_get_pan (MIXCHN *self)
{
    return self->pan;
}

void __far mixchn_set_instrument_num (MIXCHN *self, uint8_t value)
{
    self->bIns = value;
}

uint8_t __far mixchn_get_instrument_num (MIXCHN *self)
{
    return self->bIns;
}

void __far mixchn_set_instrument (MIXCHN *self, MUSINS *value)
{
    self->pMusIns = value;
}

MUSINS *__far mixchn_get_instrument (MIXCHN *self)
{
    return self->pMusIns;
}

void __far mixchn_set_sample_volume (MIXCHN *self, int16_t vol)
{
    if (vol < 0)
        self->bSmpVol = 0;
    else
        self->bSmpVol = vol > SMPVOL_MAX ? SMPVOL_MAX : vol;
}

uint8_t __far mixchn_get_sample_volume (MIXCHN *self)
{
    return self->bSmpVol;
}

void __far mixchn_set_sample_period_limits (MIXCHN *self, uint16_t rate, bool amiga)
{
    unsigned int lo, hi;

    if (amiga)
    {
         lo = getNotePeriod((5 << 4) + 11); /* B-5 */
         hi = getNotePeriod((3 << 4) +  0); /* C-3 */
    }
    else
    {
         lo = getNotePeriod((7 << 4) + 11); /* B-7 */
         hi = getNotePeriod((0 << 4) +  0); /* C-0 */
    }
     self->wSmpPeriodLow  = (unsigned long)(MID_C_RATE * (unsigned long)(lo)) / rate;
     self->wSmpPeriodHigh = (unsigned long)(MID_C_RATE * (unsigned long)(hi)) / rate;
}

uint16_t __far mixchn_check_sample_period (MIXCHN *self, uint32_t value)
{
    if (value < self->wSmpPeriodLow)
        value = self->wSmpPeriodLow;
    else
        if (value > self->wSmpPeriodHigh)
            value = self->wSmpPeriodHigh;

    return value;
}

void __far mixchn_set_sample_period (MIXCHN *self, uint16_t value)
{
    self->wSmpPeriod = value;
}

uint16_t __far mixchn_get_sample_period (MIXCHN *self)
{
    return self->wSmpPeriod;
}

void __far mixchn_set_sample_step (MIXCHN *self, uint32_t value)
{
    self->dSmpStep = value;
}

uint32_t __far mixchn_get_sample_step (MIXCHN *self)
{
    return self->dSmpStep;
}

void __far mixchn_setup_sample_period (MIXCHN *self, uint32_t value)
{
    if (value)
    {
        value = mixchn_check_sample_period(self, value);
        mixchn_set_sample_period(self, value);
        mixchn_set_sample_step(self, _calc_sample_step(value, playState_rate));
    }
    else
    {
        mixchn_set_sample_period(self, 0);
        mixchn_set_sample_step(self, 0);
    }
}

void __far mixchn_set_sample_data (MIXCHN *self, void *value)
{
    self->wSmpSeg = FP_SEG(value);
}

void *__far mixchn_get_sample_data (MIXCHN *self)
{
    return MK_FP(self->wSmpSeg, 0);
}

void __far mixchn_set_command (MIXCHN *self, uint8_t value)
{
    self->bCommand = value;
}

uint8_t __far mixchn_get_command (MIXCHN *self)
{
    return self->bCommand;
}

void __far mixchn_set_sub_command (MIXCHN *self, uint8_t value)
{
    self->bCommand2 = value;
}

uint8_t __far mixchn_get_sub_command (MIXCHN *self)
{
    return self->bCommand2;
}

void __far mixchn_set_command_parameter (MIXCHN *self, uint8_t value)
{
    self->bParameter = value;
}

uint8_t __far mixchn_get_command_parameter (MIXCHN *self)
{
    return self->bParameter;
}

void __far mixchn_reset_wave_tables (MIXCHN *self)
{
    if (self)
    {
        self->wVibTab = FP_OFF(&sinuswave);
        self->wTrmTab = FP_OFF(&sinuswave);
    }
}

void __far mixchn_free (MIXCHN *self)
{
    return;
}

void __far chn_setupInstrument (MIXCHN *chn, uint8_t insNum)
{
    MUSMOD *track;
    MUSINSLIST *instruments;
    PCMSMPLIST *samples;
    MUSINS *ins;
    PCMSMP *smp;
    unsigned int rate;
    unsigned int flags;

    track = mod_Track;
    instruments = musmod_get_instruments (track);
    samples = musmod_get_samples (track);
    ins = musinsl_get (instruments, insNum - 1);
    if (musins_get_type(ins) == MUSINST_PCM)
    {
        smp = musins_get_sample (ins);
        if (smp && pcmsmp_is_available (smp))
        {
            rate = pcmsmp_get_rate (smp);
            if (rate)
            {
                mixchn_set_instrument_num (chn, insNum);
                mixchn_set_instrument (chn, ins);
                mixchn_set_sample (chn, smp);
                mixchn_set_sample_volume (chn, (musins_get_volume (ins) * playState_gVolume) >> 6);
                flags = 0;
                if (pcmsmp_get_bits (smp) == 16)
                    flags |= MIXSMPFL_16BITS;
                if (pcmsmp_get_loop (smp) != PCMSMPLOOP_NONE)
                    flags |= MIXSMPFL_LOOP;
                chn->bSmpFlags = flags;
                chn->wSmpLoopStart = pcmsmp_get_loop_start (smp);
                if (flags & MIXSMPFL_LOOP)
                    chn->wSmpLoopEnd = pcmsmp_get_loop_end (smp);
                else
                    chn->wSmpLoopEnd = pcmsmp_get_length (smp);
                chn->wSmpStart = 0; // reset start position
                mixchn_set_sample_period_limits (chn, rate, musmod_is_amiga_limits (track));
            }
            else
                // don't play it - it's wrong !
                mixchn_set_instrument_num(chn, 0);
        }
        else
            // don't play it - it's wrong !
            mixchn_set_instrument_num(chn, 0);

    }
    else
        // don't play it - it's wrong !
        mixchn_set_instrument_num(chn, 0);
}

uint16_t __far chn_calcNotePeriod (MIXCHN *chn, uint32_t rate, uint8_t note)
{
    unsigned int period;
    period = (unsigned long)(MID_C_RATE * (unsigned long)getNotePeriod(note)) / rate;
    return mixchn_check_sample_period(chn, period);
}

uint32_t __far chn_calcNoteStep (MIXCHN *chn, uint32_t rate, uint8_t note)
{
    unsigned int period;
    period = chn_calcNotePeriod(chn, rate, note);
    if (period)
        return _calc_sample_step(period, playState_rate);
    else
        return 0;
}

void __far chn_setupNote (MIXCHN *chn, uint8_t note, bool keep)
{
    MUSINS *ins;
    PCMSMP *smp;
    uint32_t rate;

    chn->bNote = note;
    mixchn_set_sample_period(chn, 0);   // clear it first - just to make sure we really set it
    if (mixchn_get_instrument_num(chn))
    {
        ins = mixchn_get_instrument(chn);
        if (musins_get_type (ins) == MUSINST_PCM)
        {
            smp = musins_get_sample (ins);
            if (smp && pcmsmp_is_available (smp))
            {
                rate = pcmsmp_get_rate (smp);
                if (rate)
                {
                    mixchn_setup_sample_period(chn, chn_calcNotePeriod(chn, rate, note));
                    if (! keep)
                    {
                        // restart instrument
                        chn->dSmpPos = (unsigned long)chn->wSmpStart << 16;
                        mixchn_set_playing(chn, true);
                    }
                }
            }
        }
    }
}

/*** Mixing channels list ***/

#define _mixchnl_get_list(o)    & ((o)->list)

MIXCHNLFLAGS __far __mixchnl_set_flags (MIXCHNLFLAGS _flags, MIXCHNLFLAGS _mask, MIXCHNLFLAGS _set, bool raise)
{
    MIXCHNLFLAGS result;

    result = _flags & _mask;
    if (raise)
        result |= _set;

    return result;
}

void __far _mixchnl_init_item (void *self, void *item)
{
    mixchn_init ((MIXCHN *) item);
}

void __far _mixchnl_free_item (void *self, void *item)
{
    mixchn_free ((MIXCHN *) item);
}

void __far mixchnl_init (MIXCHNLIST *self)
{
    if (self)
    {
        memset (self, 0, sizeof (MIXCHNLIST));
        dynarr_init (_mixchnl_get_list (self), self, sizeof (MIXCHN), _mixchnl_init_item, _mixchnl_free_item);
    }
}

MIXCHN *__far mixchnl_get (MIXCHNLIST *self, uint16_t index)
{
    if (self)
        return dynarr_get_item (_mixchnl_get_list (self), index);
    else
        return NULL;
}

bool __far mixchnl_set_count (MIXCHNLIST *self, uint16_t value)
{
    if (self)
        return dynarr_set_size (_mixchnl_get_list (self), value);
    else
        return false;
}

uint16_t __far mixchnl_get_count (MIXCHNLIST *self)
{
    if (self)
        return dynarr_get_size (_mixchnl_get_list (self));
    else
        return 0;
}

void __far mixchnl_free (MIXCHNLIST *self)
{
    if (self)
    {
        dynarr_free (_mixchnl_get_list (self));
        mixchnl_init (self);    // clear
    }
}

#ifdef DEFINE_LOCAL_DATA

MIXCHNLIST *mod_Channels;

#endif  /* DEFINE_LOCAL_DATA */
