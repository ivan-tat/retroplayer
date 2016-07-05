/* pic.c -- Programmable Interuupt Controller interface.

   This is free and unencumbered software released into the public domain */

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#include <conio.h>
#endif

#include "pic.h"

void __far __pascal picEnableIRQs( uint16_t mask ) {
    if ( mask & 0xff ) {
        outp( 0x21, inp( 0x21 ) | ( uint8_t )( mask & 0xff ) );
    }
    if ( mask & 0xff00 ) {
        outp( 0xa1, inp( 0xa1 ) | ( uint8_t )( ( mask >> 8 ) & 0xff ) );
    }
}

void __far __pascal picDisableIRQs( uint16_t mask ) {
    if ( mask & 0xff ) {
        outp( 0x21, inp( 0x21 ) & ( uint8_t )( ~( mask & 0xff ) ) );
    }
    if ( mask & 0xff00 ) {
        outp( 0xa1, inp( 0xa1 ) & ( uint8_t )( ~( ( mask >> 8 ) & 0xff ) ) );
    }
}
