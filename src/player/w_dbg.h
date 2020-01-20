/* w_dbg.h -- declarations for w_dbg.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PLAYER_W_DBG_H_INCLUDED
#define _PLAYER_W_DBG_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "player/screen.h"

bool __far win_debug_init (SCRWIN *self);
void __far win_debug_set_player (SCRWIN *self, MUSPLAYER *value);
void __far win_debug_set_track (SCRWIN *self, MUSMOD *value);
void __far win_debug_set_play_state (SCRWIN *self, PLAYSTATE *value);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux win_debug_init "*";
#pragma aux win_debug_set_player "*";
#pragma aux win_debug_set_track "*";
#pragma aux win_debug_set_play_state "*";

#endif  /* __WATCOMC__ */

#endif  /* !_PLAYER_W_DBG_H_INCLUDED */
