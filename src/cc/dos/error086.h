/* error086.h -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_DOS_ERROR086_H_INCLUDED
#define _CC_DOS_ERROR086_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include "cc/i86.h"

unsigned __cc_doserror (union CC_REGPACK *regs);
unsigned __cc_doserror2 (void);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux __cc_doserror "*";
#pragma aux __cc_doserror2 "*";

#endif  /* __WATCOMC__ */

#endif  /* !_CC_DOS_ERROR086_H_INCLUDED */
