/* detisr_.h -- declarations for detisr_.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _DETISR__H
#define _DETISR__H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

typedef void PUBLIC_CODE DetISRCallback_t(uint8_t irq);

extern DetISRCallback_t *PUBLIC_DATA DetISRCallback;

extern void *PUBLIC_CODE GetDetISR(uint8_t irq);

#endif  /* _DETISR__H */
