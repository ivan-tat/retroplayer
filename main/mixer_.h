#ifndef MIXER__H
#define MIXER__H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"

extern uint32_t PUBLIC_CODE _mixCalcSampleStep(void);
extern void     PUBLIC_CODE _MixSampleMono8(void);
extern void     PUBLIC_CODE _MixSampleStereo8(void);

#endif /* MIXER__H */
