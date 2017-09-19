/* mixer.h -- declarations for mixer.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXER_H
#define MIXER_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "pascal/pascal.h"

extern uint32_t PUBLIC_CODE mixCalcSampleStep(uint16_t wPeriod);
#ifdef __WATCOMC__
#pragma aux mixCalcSampleStep modify [ bx cx ];
#endif

#endif /* MIXER_H */
