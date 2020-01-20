/* w_hlp.h -- declarations for w_hlp.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PLAYER_W_HLP_H_INCLUDED
#define _PLAYER_W_HLP_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "player/screen.h"

bool __far win_help_init (SCRWIN *self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux win_help_init "*";

#endif  /* __WATCOMC__ */

#endif  /* !_PLAYER_W_HLP_H_INCLUDED */
