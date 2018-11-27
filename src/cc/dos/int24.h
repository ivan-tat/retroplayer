/* int24.h -- declarations for int24.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_DOS_INT24_H
#define CC_DOS_INT24_H

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

/*
 * DOS critical error handler.
 * Interrupt is called by DOS.
 */
extern __far _cc_local_int24_asm(void);

/* Linking */

#ifdef __WATCOMC__
#pragma aux _cc_local_int24_asm "*";
#endif

#endif  /* CC_DOS_INT24_H */
