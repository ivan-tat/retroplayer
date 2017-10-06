/* mixchn.c -- mixing channel handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>

#include "main/mixvars.h"
#include "main/mixer.h"

#include "main/mixchn.h"

void PUBLIC_CODE chn_setSampleVolume(MIXCHN *chn, int16_t vol)
{
    if (vol < 0)
        chn->bSmpVol = 0;
    else
        chn->bSmpVol = vol > CHNINSVOL_MAX ? CHNINSVOL_MAX : vol;
}

uint16_t PUBLIC_CODE chn_checkSamplePeriod(MIXCHN *chn, uint32_t period)
{
	if (period < chn->wSmpPeriodLow)
		period = chn->wSmpPeriodLow;
	else
		if (period > chn->wSmpPeriodHigh)
			period = chn->wSmpPeriodHigh;

    return period;
}

void PUBLIC_CODE chn_setSamplePeriod(MIXCHN *chn, uint32_t period)
{
    if (period)
    {
		period = chn_checkSamplePeriod(chn, period);
        _chn_setSamplePeriod(chn, period);
        chn_setSampleStep(chn, mixCalcSampleStep(period));
    }
    else
    {
        _chn_setSamplePeriod(chn, 0);
        chn_setSampleStep(chn, 0);
    }
}

void PUBLIC_CODE chn_setPeriodLimits(MIXCHN *chn, uint16_t rate, bool amiga)
{
	unsigned int lo, hi;

    if (amiga)
    {
         lo = getNotePeriod((5 << 4) + 11); /* B-5 */
         hi = getNotePeriod((3 << 4) +  0); /* C-3 */
    }
    else
    {
         lo = getNotePeriod((7 << 4) + 11);	/* B-7 */
         hi = getNotePeriod((0 << 4) +  0); /* C-0 */
    }
	 chn->wSmpPeriodLow  = (unsigned long)(MID_C_RATE * (unsigned long)(lo)) / rate;
	 chn->wSmpPeriodHigh = (unsigned long)(MID_C_RATE * (unsigned long)(hi)) / rate;
}

#ifdef DEFINE_LOCAL_DATA

channelsList_t PUBLIC_DATA Channel;
uint8_t PUBLIC_DATA UsedChannels;

#endif  /* DEFINE_LOCAL_DATA */
