/* w_hlp.h -- declarations for w_hlp.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef W_HLP_H
#define W_HLP_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "player/screen.h"

bool __far win_help_init (SCRWIN *self);

#endif  /* W_HLP_H */
