/* cpu.h -- declarations for cpu.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CPU_H
#define CPU_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

extern bool PUBLIC_CODE isCPU_i386(void);

#endif /* CPU_H */
