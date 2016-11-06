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
extern uint16_t PUBLIC_DATA BPT;
    /* bytes per tick - depends on samplerate + tempo */
extern uint16_t PUBLIC_DATA TickBytesLeft;
    /* Bytes left to next Tick */

#endif /* MIXVARS_H */
