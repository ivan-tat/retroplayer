/* detisr.h -- declarations for detisr.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _DETISR_H
#define _DETISR_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "hw/sb/detisr_.h"

void __far __pascal SetDetISRCallback( DetISRCallback_t *p );

#endif /* _DETISR_H */
