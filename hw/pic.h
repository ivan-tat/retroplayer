/* Declarations for pic.

   This is free and unencumbered software released into the public domain */

#ifndef _PIC_H
#define _PIC_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

void __far __pascal picEnableIRQs( uint16_t mask );
void __far __pascal picDisableIRQs( uint16_t mask );
void __far __pascal picEOI( uint8_t irq );

#endif /* _PIC_H */
