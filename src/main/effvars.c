/* effvars.c -- variables for effects handling.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$main$effvars$*"
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "cc/i86.h"
#include "main/effvars.h"

#if DEFINE_LOCAL_DATA == 1

const int8_t __near sinuswave[64] =
{
#include "main/wsinus.inc"
};

const int8_t __near rampwave[64] =
{
#include "main/wramp.inc"
};

const uint8_t __near squarewave[64] =
{
#include "main/wsquare.inc"
};

const void __near * __near wavetab[3] =
{
    (void __near *)&sinuswave,
    (void __near *)&rampwave,
    (void __near *)&squarewave
    /* 'random wave' is not a table, but a call for a random number ! */
};

#endif  /* DEFINE_LOCAL_DATA == 1 */
