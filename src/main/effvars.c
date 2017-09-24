/* effvars.c -- variables for effects handling.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "cc/i86.h"

#include "main/effvars.h"

#ifdef DEFINE_LOCAL_DATA

static const int8_t __near sinuswave[64] =
{
#include "main/_wsinus.inc"
};

static const int8_t __near rampwave[64] =
{
#include "main/_wramp.inc"
};

static const int8_t __near squarewave[64] =
{
#include "main/_wsquare.inc"
};

static const uint8_t __near *wavetab[3] =
{
    &sinuswave,
    &rampwave,
    &squarewave
    /* 'random wave' is not a table, but a call for a random number ! */
};

#endif
