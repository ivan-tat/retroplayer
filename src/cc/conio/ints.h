/* ints.h -- declarations for ints.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_CONIO_INTS_H_INCLUDED
#define _CC_CONIO_INTS_H_INCLUDED

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

/*
 * Ctrl-Break keyboard signal handler.
 * Interrupt is called by BIOS.
 */
extern __far _cc_local_int1b_asm (void);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _cc_local_int1b_asm "*";

#endif  /* __WATCOMC__ */

#endif  /* !_CC_CONIO_INTS_H_INCLUDED */
