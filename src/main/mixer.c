/* mixer.c -- mixer functions.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "main/mixvars.h"

// TODO: remove PUBLIC_CODE macros when done.

uint32_t PUBLIC_CODE mixCalcSampleStep(uint16_t wPeriod)
{
    long long int a = (long long int) 1712 * 8363 << 16;
    long int b = (long int)wPeriod * (long int)mixSampleRate;
    b = a / b;
    return b;
}

#ifdef __WATCOMC__
#pragma aux mixCalcSampleStep modify [ bx cx ];
#endif
