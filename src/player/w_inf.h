/* w_inf.h -- declarations for w_inf.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PLAYER_W_INF_H_INCLUDED
#define _PLAYER_W_INF_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "player/screen.h"

bool __far win_information_init (SCRWIN *self);
void __far win_information_set_player (SCRWIN *self, MUSPLAYER *value);
void __far win_information_set_track (SCRWIN *self, MUSMOD *value);
void __far win_information_set_play_state (SCRWIN *self, PLAYSTATE *value);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux win_information_init "*";
#pragma aux win_information_set_player "*";
#pragma aux win_information_set_track "*";
#pragma aux win_information_set_play_state "*";

#endif  /* __WATCOMC__ */

#endif  /* !_PLAYER_W_INF_H_INCLUDED */
