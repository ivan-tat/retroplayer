/* mixvars.h -- declarations for mixvars.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXVARS_H
#define MIXVARS_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"

extern uint16_t PUBLIC_DATA ST3Periods[12];

extern uint16_t PUBLIC_DATA UseRate;
extern uint16_t PUBLIC_DATA mixTickSamplesPerChannel;
    /* Samples per channel per tick - depends on samplerate + tempo
     *   - for 8bits play it is "N of bytes per channel per tick"
     *   - for 16bits play it is "N of words per channel per tick" */
extern uint16_t PUBLIC_DATA mixTickSamplesPerChannelLeft;
    /* Samples per channel left to next Tick
     *   - for 8bits play it is "N of bytes per channel left for next tick"
     *   - for 16bits play it is "N of words per channel left for next tick" */

#endif /* MIXVARS_H */
