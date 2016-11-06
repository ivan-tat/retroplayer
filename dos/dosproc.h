/* dosproc.h -- declarations for dosproc.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _DOSPROC_H
#define _DOSPROC_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

bool __far __pascal getdosmem( void *p, uint32_t size );
void __far __pascal freedosmem( void *p );
uint16_t __far __pascal getfreesize( void );
bool __far __pascal setsize( void *p, uint32_t size );

#endif /* _DOSPROC_H */
