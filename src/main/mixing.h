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
#include "main/mixer.h"

void sound_fill_buffer(MIXBUF *mb, uint16_t len);

#ifdef __WATCOMC__
#pragma aux sound_fill_buffer "*";
#endif

#endif  /* MIXING_H */
