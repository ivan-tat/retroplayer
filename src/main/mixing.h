/* mixing.h -- declarations for mixing.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXING_H
#define MIXING_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "main/musmod.h"
#include "main/mixer.h"
#include "main/s3mvars.h"

void sound_fill_buffer (MUSMOD *track, PLAYSTATE *ps, MIXBUF *mb, uint16_t len);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux sound_fill_buffer "*";

#endif  /* __WATCOMC__ */

#endif  /* MIXING_H */
