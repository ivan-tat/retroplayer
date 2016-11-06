/* detisr.c -- hardware IRQ detection library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "detisr.h"

void __far __pascal SetDetISRCallback( DetISRCallback_t *p ) {
	DetISRCallback = p;
}
