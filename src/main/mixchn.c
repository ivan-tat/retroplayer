/* mixchn.c -- mixing channel handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>

#include "main/mixvars.h"
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

void PUBLIC_CODE mixchn_set_instrument(MIXCHN *self, struct instrument_t *value)
{
    self->wInsSeg = FP_SEG((void __far *)value);
}

struct instrument_t *PUBLIC_CODE mixchn_get_instrument(MIXCHN *self)
{
    return (struct instrument_t *)MK_FP(self->wInsSeg, 0);
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
        mixchn_set_sample_step(self, mixCalcSampleStep(value));
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

#ifdef DEFINE_LOCAL_DATA

channelsList_t PUBLIC_DATA Channel;
uint8_t PUBLIC_DATA UsedChannels;

#endif  /* DEFINE_LOCAL_DATA */
