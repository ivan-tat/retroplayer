/* dosret.h -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_DOS_DOSRET_H
#define CC_DOS_DOSRET_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

unsigned __cc_set_errno_dos(unsigned code);

/*** Linkning ***/

#ifdef __WATCOMC__

#pragma aux __cc_set_errno_dos "^";

#endif  /* __WATCOMC__ */

#endif  /* CC_DOS_DOSRET_H */
