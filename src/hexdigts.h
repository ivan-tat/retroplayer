/* hexdigts.h -- declarations for hexdigts.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef HEXDIGTS_H
#define HEXDIGTS_H

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

extern const char HEXDIGITS[16];

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux HEXDIGITS "*";

#endif  /* __WATCOMC__ */

#endif  /* HEXDIGTS_H */
