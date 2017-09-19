/* isr_t.h -- type declarations for isr.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef ISR_T_H
#define ISR_T_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done.

#include "pascal/pascal.h"

/* Here we use "__far __pascal" modificators instead of PUBLIC_CODE:
   when all done we must use only "__far" modificator */
typedef void __far __pascal isrCallback_t(uint8_t ch);

#endif  /* ISR_T_H */
