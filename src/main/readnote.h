/* readnote.h -- declarations for readnote.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _READNOTE_H_INCLUDED
#define _READNOTE_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "main/musmod.h"

void __far readnewnotes (PLAYSTATE *ps);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux readnewnotes "*";

#endif  /* __WATCOMC__ */

#endif  /* !_READNOTE_H_INCLUDED */
