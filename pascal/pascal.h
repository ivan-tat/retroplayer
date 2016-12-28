/* pascal.h -- support for pascal linker.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef PASCAL_H
#define PASCAL_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

#define PUBLIC_DATA __pascal
#define PUBLIC_CODE __far __pascal
#define EXTERN_LINK extern

/*** System unit ***/

/* Heap */

extern uint32_t PUBLIC_CODE mavail(void);
extern void    *PUBLIC_CODE malloc(uint16_t size);
extern void     PUBLIC_CODE memfree(void *p, uint16_t size);

#endif  /* PASCAL_H */
