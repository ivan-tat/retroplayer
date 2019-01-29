/* readnote.h -- declarations for readnote.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef READNOTE_H
#define READNOTE_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "main/musmod.h"

void __far readnewnotes (MUSMOD *track, PLAYSTATE *ps, MIXCHNLIST *channels);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux readnewnotes "*";

#endif  /* __WATCOMC__ */

#endif  /* READNOTE_H */
