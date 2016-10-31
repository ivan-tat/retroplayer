/* mixer.c -- mixer functions.

   This is free and unencumbered software released into the public domain. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "s3mplay.h"

uint32_t __far __pascal mixCalcSampleStep( uint16_t period )
{
    long long int a = ( long long int ) 1712 * 8363 << 16;
    long int b = ( long int ) period * ( long int ) UseRate;
    b = a / b;
    return b;
}
