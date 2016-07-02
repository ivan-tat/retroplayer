/* Declarations for System unit.

   This is free and unencumbered software released into the public domain */

#ifndef _PASCAL_SYSWRAP_H
#define _PASCAL_SYSWRAP_H 1

#ifdef __WATCOMC__
#include <stdint.h>
#endif

/* Heap */

extern uint32_t __far __pascal mavail( void );
extern void * __far __pascal malloc( uint16_t size );
extern void __far __pascal memfree( void *p, uint16_t size );

#endif /* _PASCAL_SYSWRAP_H */
