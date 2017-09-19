/* effvars.c -- variables for effects handling.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#endif

int8_t  sinuswave[64];
int8_t  rampwave[64];
uint8_t squarewave[64];

static const uint16_t wavetab[3] = {
    MK_OFF(&sinuswave),
    MK_OFF(&rampwave),
    MK_OFF(&squarewave) /* looks not like a square but anyway */
    /* 'random wave' is not a table, but a call for a random number ! */
};
