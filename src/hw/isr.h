/* isr_.h -- declarations for isr.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef ISR_H
#define ISR_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

// TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done.

extern void *PUBLIC_CODE getISR(uint8_t ch);

#endif  /* ISR_H */
