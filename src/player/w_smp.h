/* w_smp.h -- declarations for w_smp.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PLAYER_W_SMP_H_INCLUDED
#define _PLAYER_W_SMP_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "player/screen.h"

bool __far win_samples_init (SCRWIN *self);
void __far win_samples_set_track (SCRWIN *self, MUSMOD *value);
void __far win_samples_set_channels (SCRWIN *self, MIXCHNLIST *value);
void __far win_samples_set_page_start (SCRWIN *self, int value);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux win_samples_init "*";
#pragma aux win_samples_set_track "*";
#pragma aux win_samples_set_channels "*";
#pragma aux win_samples_set_page_start "*";

#endif  /* __WATCOMC__ */

#endif  /* !_PLAYER_W_SMP_H_INCLUDED */
