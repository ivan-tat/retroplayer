/* hexdigts.h -- declarations for hexdigts.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _HEXDIGTS_H_INCLUDED
#define _HEXDIGTS_H_INCLUDED

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

extern const char HEXDIGITS[16];

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux HEXDIGITS "*";

#endif  /* __WATCOMC__ */

#endif  /* !_HEXDIGTS_H_INCLUDED */
