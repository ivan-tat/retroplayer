/* mixer_.h -- declarations for mixer_.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXER__H
#define MIXER__H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"

extern uint32_t PUBLIC_CODE _mixCalcSampleStep(uint16_t wPeriod);
extern void     PUBLIC_CODE _MixSampleMono8(void);
extern void     PUBLIC_CODE _MixSampleStereo8(void);

#endif /* MIXER__H */
