/* effvars.h -- declarations for effvars.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef EFFVARS_H
#define EFFVARS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

extern int8_t   sinuswave[64];
extern int8_t   rampwave[64];
extern uint8_t  squarewave[64];
extern uint16_t wavetab[3];

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux sinuswave "*";
#pragma aux rampwave "*";
#pragma aux squarewave "*";
#pragma aux wavetab "*";

#endif  /* __WATCOMC__ */

#endif /* EFFVARS_H */
