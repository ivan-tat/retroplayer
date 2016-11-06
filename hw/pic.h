/* pic.h -- declarations for pic.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef PIC_H
#define PIC_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

void  PUBLIC_CODE picEnableIRQs(uint16_t mask);
void  PUBLIC_CODE picDisableIRQs(uint16_t mask);
void  PUBLIC_CODE picEOI(uint8_t irq);
void *PUBLIC_CODE picGetIntVec(uint8_t irq);
void  PUBLIC_CODE picSetIntVec(uint8_t irq, void *p);

#endif /* PIC_H */
