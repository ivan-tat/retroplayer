/* ints.h -- declarations for ints.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CONIO_INTS_H
#define CONIO_INTS_H

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

/*
 * Ctrl-Break keyboard signal handler.
 * Interrupt is called by BIOS.
 */
extern __far _cc_local_int1b_asm (void);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _cc_local_int1b_asm "*";

#endif  /* __WATCOMC__ */

#endif  /* CONIO_INTS_H */
