/* mixer.c -- mixer functions.

   This is free and unencumbered software released into the public domain. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "..\pascal\pascal.h"
#include "mixvars.h"

uint32_t PUBLIC_CODE mixCalcSampleStep(uint16_t period)
{
    long long int a = (long long int) 1712 * 8363 << 16;
    long int b = (long int)period * (long int)UseRate;
    b = a / b;
    return b;
}
