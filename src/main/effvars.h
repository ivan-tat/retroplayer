/* effvars.h -- declarations for effvars.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _EFFVARS_H_INCLUDED
#define _EFFVARS_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"

extern const int8_t  __near sinuswave[64];
extern const int8_t  __near rampwave[64];
extern const uint8_t __near squarewave[64];
extern const void __near * __near wavetab[3];

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux sinuswave "*";
#pragma aux rampwave "*";
#pragma aux squarewave "*";
#pragma aux wavetab "*";

#endif  /* __WATCOMC__ */

#endif /* !_EFFVARS_H_INCLUDED */
