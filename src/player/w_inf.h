/* w_inf.h -- declarations for w_inf.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef W_INF_H
#define W_INF_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "player/screen.h"

void __far win_information_init(SCRWIN *self);
void __far win_information_draw(SCRWIN *self);

#endif  /* W_INF_H */
