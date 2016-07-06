/* Declarations for CRT unit.

   This is free and unencumbered software released into the public domain */

#ifndef _PASCAL_CRT_H
#define _PASCAL_CRT_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

extern void __far __pascal delay( uint16_t count );

#endif /* _PASCAL_CRT_H */
