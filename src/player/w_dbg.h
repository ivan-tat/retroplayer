/* w_dbg.h -- declarations for w_dbg.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef W_DBG_H
#define W_DBG_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "player/screen.h"

bool __far win_debug_init (SCRWIN *self);

#endif  /* W_DBG_H */
