/* mixing.h -- declarations for mixing.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _MIXING_H_INCLUDED
#define _MIXING_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "main/musmod.h"
#include "main/mixchn.h"
#include "main/musmodps.h"
#include "main/mixer.h"

void song_play (PLAYSTATE *ps, MIXER *mixer, uint16_t len);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux song_play "*";

#endif  /* __WATCOMC__ */

#endif  /* !_MIXING_H_INCLUDED */
