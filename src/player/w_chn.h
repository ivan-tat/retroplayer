/* w_chn.h -- declarations for w_chn.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef W_CHN_H
#define W_CHN_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "player/screen.h"

void __far win_channels_init(SCRWIN *self);
void __far win_channels_draw(SCRWIN *self);

#endif  /* W_CHN_H */
