/* w_chn.h -- declarations for w_chn.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PLAYER_W_CHN_H_INCLUDED
#define _PLAYER_W_CHN_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "player/screen.h"

bool __far win_channels_init (SCRWIN *self);
void __far win_channels_set_channels (SCRWIN *self, MIXCHNLIST *value);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux win_channels_init "*";
#pragma aux win_channels_set_channels "*";

#endif  /* __WATCOMC__ */

#endif  /* !_PLAYER_W_CHN_H_INCLUDED */
