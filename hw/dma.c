/* dma.c -- Intel 8237 DMA controller interface.

   This is free and unencumbered software released into the public domain */

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#include <conio.h>
#endif

#include "dma.h"

/* DMA i/o ports */

typedef struct DMAIO_t {
    uint8_t mask;
    uint8_t clear;
    uint8_t mode;
    uint8_t addr;
    uint8_t page;
    uint8_t count;
};

const static struct DMAIO_t DMAIO[8] = {
    /* 8-bits transfers */
    { 0x0a, 0x0c, 0x0b, 0x00, 0x87, 0x01 },
    { 0x0a, 0x0c, 0x0b, 0x02, 0x83, 0x03 },
    { 0x0a, 0x0c, 0x0b, 0x04, 0x81, 0x05 },
    { 0x0a, 0x0c, 0x0b, 0x06, 0x82, 0x07 },
    /* 16-bits transfers */
    { 0xd4, 0xd8, 0xd6, 0xc0, 0x8f, 0xc2 },
    { 0xd4, 0xd8, 0xd6, 0xc4, 0x8b, 0xc6 },
    { 0xd4, 0xd8, 0xd6, 0xc8, 0x89, 0xca },
    { 0xd4, 0xd8, 0xd6, 0xcc, 0x8a, 0xce }
};

/* mask */

#define MASK_CHAN 0x03
#define MASK_MASK 0x04

void __far __pascal dmaMask( uint8_t ch ) {
    /* mask channel */
    outp( DMAIO[ch].mask, ( ch & MASK_CHAN ) | MASK_MASK );
}

void __far __pascal dmaEnable( uint8_t ch ) {
    /* enable channel */
    outp( DMAIO[ch].mask, ch & MASK_CHAN );
}

uint32_t dmaGetLinearAddr( void *p ) {
    return ( ( uint32_t )( FP_SEG( p ) ) << 4 ) + FP_OFF( p );
}

void __far __pascal dmaSetup( uint8_t ch, DMAMode_t mode, void *p, uint16_t count ) {
    uint32_t linear = dmaGetLinearAddr( p );
    uint16_t addr;
    uint8_t page;

    dmaMask( ch );
    
    /* clear flip-flop */
    outp( DMAIO[ch].clear, 0 );

    /* set mode */
    outp( DMAIO[ch].mode, ( mode & ( ~DMA_MODE_CHAN_MASK ) | ( ch & DMA_MODE_CHAN_MASK ) ) );

    if ( ch < 4 ) {
        addr = linear & 0xffff;
        page = ( linear >> 16 ) & 0xff;
    } else {
        /* addr is in 16-bit values */
        addr = ( linear >> 1 ) & 0xffff;
        /* page address is the same but now it accesses 128 KiB continously */
        page = ( linear >> 16 ) & 0xfe;
    }
    count--;

    /* set memory addr */
    outp( DMAIO[ch].addr, addr & 0xff );
    outp( DMAIO[ch].addr, ( addr >> 8 ) & 0xff );

    /* set memory page */
    outp( DMAIO[ch].page, page );

    /* set count */
    outp( DMAIO[ch].count, count & 0xff );
    outp( DMAIO[ch].count, ( count >> 8 ) & 0xff );

    dmaEnable( ch );
}

uint16_t __far __pascal dmaGetCounter( uint8_t ch ) {
    uint8_t lo, hi;

    /* clear flip-flop */
    outp( DMAIO[ch].clear, 0 );

    lo = inp( DMAIO[ch].count );
    hi = inp( DMAIO[ch].count );
    /* bytes|words left to send = result + 1 */

    return lo + ( hi << 8 );
}
