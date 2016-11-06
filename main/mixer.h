#ifndef MIXER_H
#define MIXER_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"

extern uint32_t PUBLIC_CODE mixCalcSampleStep(uint16_t period);

#endif /* MIXER_H */
