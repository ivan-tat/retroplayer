/* error086.h -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_DOS_ERROR086_H
#define CC_DOS_ERROR086_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include "cc/i86.h"

unsigned __cc_doserror (union CC_REGPACK *regs);
unsigned __cc_doserror2 (void);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux __cc_doserror "*";
#pragma aux __cc_doserror2 "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_DOS_ERROR086_H */
