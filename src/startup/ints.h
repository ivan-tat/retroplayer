/* ints.h -- declarations for ints.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef STARTUP_INTS_H
#define STARTUP_INTS_H

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

/*
 * Division by zero signal handler.
 * Interrupt is called by hardware.
 */
extern __far _cc_local_int0_asm(void);

/*
 * Ctrl-Break signal handler.
 * Interrupt is called by DOS.
 */
extern __far _cc_local_int23_asm(void);

// No return.
void __far __stdcall _cc_local_int0(void __far *addr, uint16_t flags);

// No return.
void __far __stdcall _cc_local_int23(void __far *addr, uint16_t flags);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _cc_local_int0_asm "*";
#pragma aux _cc_local_int23_asm "*";
#pragma aux _cc_local_int0 "*";
#pragma aux _cc_local_int23 "*";

#endif  /* __WATCOMC__ */

#endif  /* STARTUP_INTS_H */
