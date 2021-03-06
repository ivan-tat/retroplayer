/* cpu.h -- declarations for cpu.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CPU_H_INCLUDED
#define _CPU_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"

extern bool isCPU_8086(void);
extern bool isCPU_i386(void);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux isCPU_8086 "*";
#pragma aux isCPU_i386 "*";

#endif  /* __WATCOMC__ */

#endif /* !_CPU_H_INCLUDED */
