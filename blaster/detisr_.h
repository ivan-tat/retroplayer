/* detisr_.h -- declarations for detisr_.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _DETISR__H
#define _DETISR__H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

typedef void __far __pascal DetISRCallback_t( uint8_t irq );

extern DetISRCallback_t * __pascal DetISRCallback;

extern void *__far __pascal GetDetISR( uint8_t irq );

#endif /* _DETISR__H */
