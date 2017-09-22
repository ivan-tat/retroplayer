/* error086.h -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_DOS_ERROR086_H
#define CC_DOS_ERROR086_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

unsigned __cc_doserror(unsigned code);

/* Linking */

#ifdef __WATCOMC__

#pragma aux __cc_doserror "^";

#endif

#endif  /* CC_DOS_ERROR086_H */
