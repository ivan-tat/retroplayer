/* w_pat.h -- declarations for w_pat.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef W_PAT_H
#define W_PAT_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "player/screen.h"

void __far win_pattern_init(SCRWIN *self);
void __far win_pattern_draw(SCRWIN *self);
bool __far win_pattern_keypress(SCRWIN *self, char c);

#endif  /* W_PAT_H */
