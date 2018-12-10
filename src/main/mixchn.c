/* mixchn.c -- mixing channel handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>

#include "main/effvars.h"
#include "main/effects.h"
#include "main/s3mvars.h"
#include "main/mixer.h"

#include "main/mixchn.h"

void PUBLIC_CODE mixchn_set_flags(MIXCHN *self, MIXCHNFLAGS value)
{
    self->bChannelFlags = value;
}

MIXCHNFLAGS PUBLIC_CODE mixchn_get_flags(MIXCHN *self)
{
    return self->bChannelFlags;
}

void PUBLIC_CODE mixchn_set_enabled(MIXCHN *self, bool value)
{
    if (value)
        self->bChannelFlags |= MIXCHNFL_ENABLED;
    else
        self->bChannelFlags &= ~MIXCHNFL_ENABLED;
}

bool PUBLIC_CODE mixchn_is_enabled(MIXCHN *self)
{
    return (self->bChannelFlags & MIXCHNFL_ENABLED) != 0;
}

void PUBLIC_CODE mixchn_set_playing(MIXCHN *self, bool value)
{
    if (value)
        self->bChannelFlags |= MIXCHNFL_PLAYING;
    else
        self->bChannelFlags &= ~MIXCHNFL_PLAYING;
}

bool PUBLIC_CODE mixchn_is_playing(MIXCHN *self)
{
    return (self->bChannelFlags & MIXCHNFL_PLAYING) != 0;
}

void PUBLIC_CODE mixchn_set_mixing(MIXCHN *self, bool value)
{
    if (value)
        self->bChannelFlags |= MIXCHNFL_MIXING;
    else
        self->bChannelFlags &= ~MIXCHNFL_MIXING;
}

bool PUBLIC_CODE mixchn_is_mixing(MIXCHN *self)
{
    return (self->bChannelFlags & MIXCHNFL_MIXING) != 0;
}

void PUBLIC_CODE mixchn_set_type(MIXCHN *self, uint8_t value)
{
    self->bChannelType = value;
}

uint8_t PUBLIC_CODE mixchn_get_type(MIXCHN *self)
{
    return self->bChannelType;
}

void PUBLIC_CODE mixchn_set_instrument_num(MIXCHN *self, uint8_t value)
{
    self->bIns = value;
}

uint8_t PUBLIC_CODE mixchn_get_instrument_num(MIXCHN *self)
{
    return self->bIns;
}

void PUBLIC_CODE mixchn_set_instrument(MIXCHN *self, MUSINS *value)
{
    self->pMusIns = value;
}

MUSINS *PUBLIC_CODE mixchn_get_instrument(MIXCHN *self)
{
    return self->pMusIns;
}

void PUBLIC_CODE mixchn_set_sample_volume(MIXCHN *self, int16_t vol)
{
    if (vol < 0)
        self->bSmpVol = 0;
    else
        self->bSmpVol = vol > CHNINSVOL_MAX ? CHNINSVOL_MAX : vol;
}

uint8_t PUBLIC_CODE mixchn_get_sample_volume(MIXCHN *self)
{
    return self->bSmpVol;
}

void PUBLIC_CODE mixchn_set_sample_period_limits(MIXCHN *self, uint16_t rate, bool amiga)
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

uint16_t PUBLIC_CODE mixchn_check_sample_period(MIXCHN *self, uint32_t value)
{
    if (value < self->wSmpPeriodLow)
        value = self->wSmpPeriodLow;
    else
        if (value > self->wSmpPeriodHigh)
            value = self->wSmpPeriodHigh;

    return value;
}

void PUBLIC_CODE mixchn_set_sample_period(MIXCHN *self, uint16_t value)
{
    self->wSmpPeriod = value;
}

uint16_t PUBLIC_CODE mixchn_get_sample_period(MIXCHN *self)
{
    return self->wSmpPeriod;
}

void PUBLIC_CODE mixchn_set_sample_step(MIXCHN *self, uint32_t value)
{
    self->dSmpStep = value;
}

uint32_t PUBLIC_CODE mixchn_get_sample_step(MIXCHN *self)
{
    return self->dSmpStep;
}

void PUBLIC_CODE mixchn_setup_sample_period(MIXCHN *self, uint32_t value)
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

void PUBLIC_CODE mixchn_set_sample_data(MIXCHN *self, void *value)
{
    self->wSmpSeg = FP_SEG(value);
}

void *PUBLIC_CODE mixchn_get_sample_data(MIXCHN *self)
{
    return MK_FP(self->wSmpSeg, 0);
}

void PUBLIC_CODE mixchn_set_command(MIXCHN *self, uint8_t value)
{
    self->bCommand = value;
}

uint8_t PUBLIC_CODE mixchn_get_command(MIXCHN *self)
{
    return self->bCommand;
}

void PUBLIC_CODE mixchn_set_sub_command(MIXCHN *self, uint8_t value)
{
    self->bCommand2 = value;
}

uint8_t PUBLIC_CODE mixchn_get_sub_command(MIXCHN *self)
{
    return self->bCommand2;
}

void PUBLIC_CODE mixchn_set_command_parameter(MIXCHN *self, uint8_t value)
{
    self->bParameter = value;
}

uint8_t PUBLIC_CODE mixchn_get_command_parameter(MIXCHN *self)
{
    return self->bParameter;
}

void PUBLIC_CODE mixchn_reset_wave_tables(MIXCHN *self)
{
    if (self)
    {
        self->wVibTab = FP_OFF(&sinuswave);
        self->wTrmTab = FP_OFF(&sinuswave);
    }
}

void PUBLIC_CODE chn_setupInstrument(MIXCHN *chn, uint8_t insNum)
{
    MUSINS *ins;
    unsigned int rate;
    unsigned int flags;

    ins = musinsl_get(mod_Instruments, insNum - 1);
    if (musins_get_type(ins) == MUSINST_PCM)
    {
        rate = musins_get_rate(ins);
        if (rate)
        {
            mixchn_set_instrument_num(chn, insNum);
            mixchn_set_instrument(chn, ins);
            mixchn_set_sample_volume(chn, (musins_get_volume(ins) * playState_gVolume) >> 6);
            flags = 0;
            if (musins_is_looped(ins))
                flags |= SMPFLAG_LOOP;
            chn->bSmpFlags = flags;
            chn->wSmpLoopStart = musins_get_loop_start(ins);
            if (musins_is_looped(ins))
                chn->wSmpLoopEnd = musins_get_loop_end(ins);
            else
                chn->wSmpLoopEnd = musins_get_length(ins);
            chn->wSmpStart = 0; // reset start position
            mixchn_set_sample_period_limits(chn, rate, modOption_AmigaLimits);
        }
        else
            // don't play it - it's wrong !
            mixchn_set_instrument_num(chn, 0);
    }
    else
        // don't play it - it's wrong !
        mixchn_set_instrument_num(chn, 0);
}

uint16_t PUBLIC_CODE chn_calcNotePeriod(MIXCHN *chn, uint32_t rate, uint8_t note)
{
    unsigned int period;
    period = (unsigned long)(MID_C_RATE * (unsigned long)getNotePeriod(note)) / rate;
    return mixchn_check_sample_period(chn, period);
}

uint32_t PUBLIC_CODE chn_calcNoteStep(MIXCHN *chn, uint32_t rate, uint8_t note)
{
    unsigned int period;
    period = chn_calcNotePeriod(chn, rate, note);
    if (period)
        return _calc_sample_step(period, playState_rate);
    else
        return 0;
}

void PUBLIC_CODE chn_setupNote(MIXCHN *chn, uint8_t note, bool keep)
{
    MUSINS *ins;
    uint32_t rate;

    chn->bNote = note;
    mixchn_set_sample_period(chn, 0);   // clear it first - just to make sure we really set it
    if (mixchn_get_instrument_num(chn))
    {
        ins = mixchn_get_instrument(chn);
        if (musins_get_type(ins) == MUSINST_PCM)
        {
            rate = musins_get_rate(ins);
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

#ifdef DEFINE_LOCAL_DATA

channelsList_t mod_Channels;
uint8_t mod_ChannelsCount;

#endif  /* DEFINE_LOCAL_DATA */
