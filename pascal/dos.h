/* dos.h -- declarations for Pascal DOS unit.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _PASCAL_DOS_H
#define _PASCAL_DOS_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

extern void __far __pascal getintvec( uint8_t num, void **p );
extern void __far __pascal setintvec( uint8_t num, void *p );

#endif /* _PASCAL_DOS_H */
