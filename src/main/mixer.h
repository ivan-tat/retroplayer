/* mixer.h -- declarations for mixer.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXER_H
#define MIXER_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

// TODO: remove PUBLIC_CODE macros when done.

extern uint32_t PUBLIC_CODE _calc_sample_step(uint16_t wPeriod);

#ifdef __WATCOMC__
#pragma aux _calc_sample_step modify [ bx cx ];
#endif

#endif /* MIXER_H */
