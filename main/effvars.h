/* effects.h -- declarations for effects.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef EFFVARS_H
#define EFFVARS_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove reserved words "extern" and PUBLIC_DATA macros when done.

#include "..\pascal\pascal.h"

extern int8_t   PUBLIC_DATA sinuswave[64];
extern int8_t   PUBLIC_DATA rampwave[64];
extern uint8_t  PUBLIC_DATA squarewave[64];
extern uint16_t PUBLIC_DATA wavetab[3];

#endif /* EFFVARS_H */
