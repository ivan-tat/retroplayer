/* Declarations for sbctl.

   Sound Blaster hardware control library.

   This is free and unencumbered software released into the public domain */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#include <conio.h>
#endif

#include "..\pascal\crt.h"
#include "..\hw\dma.h"
#include "sbio.h"
#include "sbctl.h"

/* This routine may not work for all registers because of different timings. */
void __far __pascal sbMixerWrite( uint8_t reg, uint8_t data ) {
    /* SB 1.0/1.5/2.0/2.5 has no mixer */
    if ( ! ( sbno == 1 || sbno == 3 ) )
        sbioMixerWrite( sdev_hw_base, reg, data );
}

uint8_t __far __pascal sbMixerRead( uint8_t reg ) {
    /* SB 1.0/1.5/2.0/2.5 has no mixer */
    if ( ! ( sbno == 1 || sbno == 3 ) )
        return sbioMixerRead( sdev_hw_base, reg );
    else
        return 0;
}

void __far __pascal speaker_on( void ) {
    sbioDSPWrite( sdev_hw_base, 0xd1 );
    /* needs a bit time to switch it on */
    delay( 110 );
}

void __far __pascal speaker_off( void ) {
    sbioDSPWrite( sdev_hw_base, 0xd3 );
    /* needs a bit time to switch it off */
    delay( 220 );
}

void __far __pascal sbSetupDSPTransfer( uint16_t len, bool autoinit ) {
    uint8_t cmd, mode;
    
    if ( sbno == 6 ) {
        len--;
        if ( _16bit ) {
            /* DSP 0xB6 - use 16bit autoinit */
            /* DSP 0xB2 - use 16bit nonautoinit */
            cmd = autoinit ? 0xb6 : 0xb2;
        } else {
            /* DSP 0xC6 - use 8bit autoinit */
            /* DSP 0xC2 - use 8bit nonautoinit */
            cmd = autoinit ? 0xc6 : 0xc2;
        }
        sbioDSPWrite( sdev_hw_base, cmd );
        mode = 0;
        /* 2nd command byte: bit 4 = 1 - signed data */
        if ( sdev_mode_sign ) mode |= 0x10;
        /* 2nd command byte: bit 5 = 1 - stereo data */
        if ( stereo ) mode |= 0x20;
        sbioDSPWrite( sdev_hw_base, mode );
        sbioDSPWrite( sdev_hw_base, len & 0xff );
        sbioDSPWrite( sdev_hw_base, ( len >> 8 ) & 0xff );
    } else {
        len--;
        /* DSP 48h - setup DMA buffer size */
        sbioDSPWrite( sdev_hw_base, 0x48 );
        sbioDSPWrite( sdev_hw_base, len & 0xff );
        sbioDSPWrite( sdev_hw_base, ( len >> 8 ) & 0xff );
        if ( sbno == 1 ) {
            /* for SB1.0 : */
            /* DSP 0x1C - autoinit normal DMA */
            /* DSP 0x14 - nonautoinit normal DMA */
            cmd = autoinit ? 0x1c : 0x14;
        } else {
            /* >SB1.0 use highspeed modes */
            /* DSP 0x90 - autoinit highspeed DMA */
            /* DSP 0x91 - nonautoinit highspeed DMA */
            cmd = autoinit ? 0x90 : 0x91;
        }
        sbioDSPWrite( sdev_hw_base, cmd );
    }
}

void __far __pascal sbSetupDMATransfer( void *p, uint16_t count, bool autoinit ) {
    DMAMode_t mode;

    mode = DMA_MODE_TRAN_READ | DMA_MODE_ADDR_INCR | DMA_MODE_SINGLE;
    mode |= autoinit ? DMA_MODE_INIT_AUTO : DMA_MODE_INIT_SINGLE;

    if ( ! _16bit ) {
        /* first the SBPRO stereo bugfix : */
        if ( stereo ) {
            if ( sbno < 6 ) {
                /* well ... should be a SB PRO in stereo mode ... */
                /* let's send one byte - nothing but silence */
                sbioDSPWrite( sdev_hw_base, 0x10 );
                sbioDSPWrite( sdev_hw_base, 0x80 );
            }
        }
    };

    dmaSetup( _16bit ? sdev_hw_dma16 : sdev_hw_dma8, mode, p, count );
}

uint16_t __far __pascal sbGetDMACounter( void ) {
    return dmaGetCounter( _16bit ? sdev_hw_dma16 : sdev_hw_dma8 );
}
