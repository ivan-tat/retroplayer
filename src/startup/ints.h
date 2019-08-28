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
#include "startup.h"

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

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _cc_local_int0_asm "*";
#pragma aux _cc_local_int23_asm "*";

#endif  /* __WATCOMC__ */

#endif  /* STARTUP_INTS_H */
