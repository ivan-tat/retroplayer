/* dosexe16.h -- declarations for dosexe16.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef DOSEXE16_H_INCLUDED
#define DOSEXE16_H_INCLUDED 1

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

void __far _start_asm (void);
extern void __far _start_c (void);

#ifdef __WATCOMC__
#pragma aux _start_asm "*";
#pragma aux _start_c "*";
#endif

#endif  /* DOSEXE16_H_INCLUDED */
