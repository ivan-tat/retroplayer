/* Declarations for sbctl.

   Sound Blaster hardware control library.

   This is free and unencumbered software released into the public domain */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#include <conio.h>
#endif

#include "sbio.h"
#include "sbctl.h"

/* This routine may not work for all registers because of different timings. */
void __far __pascal sbMixerWrite( uint8_t reg, uint8_t data ) {
    /* SB 1.0/1.5/2.0/2.5 has no mixer */
    if ( ! ( sbno == 1 || sbno == 3 ) )
        sbioMixerWrite( dsp_addr, reg, data );
}

uint8_t __far __pascal sbMixerRead( uint8_t reg ) {
    /* SB 1.0/1.5/2.0/2.5 has no mixer */
    if ( ! ( sbno == 1 || sbno == 3 ) )
        return sbioMixerRead( dsp_addr, reg );
    else
        return 0;
}

void __far __pascal sbSetupDSPTransfer( uint16_t len, bool b16, bool autoinit ) {
    uint8_t cmd, mode;
    
    if ( sbno == 6 ) {
        len--;
        if ( b16 ) {
            /* DSP 0xB6 - use 16bit autoinit */
            /* DSP 0xB2 - use 16bit nonautoinit */
            cmd = autoinit ? 0xb6 : 0xb2;
        } else {
            /* DSP 0xC6 - use 8bit autoinit */
            /* DSP 0xC2 - use 8bit nonautoinit */
            cmd = autoinit ? 0xc6 : 0xc2;
        }
        sbioDSPWrite( dsp_addr, cmd );  
        mode = 0;
        /* 2nd command byte: bit 4 = 1 - signed data */
        if ( signeddata ) mode |= 0x10;
        /* 2nd command byte: bit 5 = 1 - stereo data */
        if ( stereo ) mode |= 0x20;
        sbioDSPWrite( dsp_addr, mode );
        sbioDSPWrite( dsp_addr, len & 0xff );
        sbioDSPWrite( dsp_addr, ( len >> 8 ) & 0xff );
    } else {
        len--;
        /* DSP 48h - setup DMA buffer size */
        sbioDSPWrite( dsp_addr, 0x48 );
        sbioDSPWrite( dsp_addr, len & 0xff );
        sbioDSPWrite( dsp_addr, ( len >> 8 ) & 0xff );
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
        sbioDSPWrite( dsp_addr, cmd );
    }
}
