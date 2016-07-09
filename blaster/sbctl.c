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

uint16_t __far __pascal sbReadDSPVersion( void ) {
    uint8_t v_lo, v_hi;

    /* DSP 0xE1 - get DSP version */
    if ( ! sbioDSPWrite( sdev_hw_base, 0xe1 ) ) return 0;

    v_hi = sbioDSPRead( sdev_hw_base );
    if ( sbioError != E_SBIO_SUCCESS ) return 0;

    v_lo = sbioDSPRead( sdev_hw_base );
    if ( sbioError != E_SBIO_SUCCESS ) return 0;

    return v_lo + ( v_hi << 8 );
}

void __far __pascal sbSetSpeaker( bool state ) {
    if ( state ) {
        /* Does not work on SB16 */
        sbioDSPWrite( sdev_hw_base, 0xd1 );
        /* Needs a bit time to switch it on */
        delay( 110 );
    } else {
        sbioDSPWrite( sdev_hw_base, 0xd3 );
        /* Needs a bit time to switch it off */
        delay( 220 );
    }
}

void adjustRate( uint16_t *rate, bool stereo, uint8_t *tc ) {
    if ( ( sbno == 6 ) || ! stereo ) {
        *tc = 256 - 1000000 / *rate;
        *rate = 1000000 / ( 256 - *tc );
    } else {
        *tc = 256 - 1000000 / ( 2 * *rate );
        *rate = ( 1000000 / ( 256 - *tc ) ) / 2;
    }
}

void __far __pascal sbAdjustMode( uint16_t *rate, bool *stereo, bool *_16bit ) {
    uint8_t tc;

    *stereo = *stereo & sdev_caps_stereo;
    *_16bit = *_16bit & sdev_caps_16bit;
    if ( *stereo ) {
        if ( *rate < 4000 ) *rate = 4000;
        if ( *rate > sdev_caps_stereo_maxrate ) *rate = sdev_caps_stereo_maxrate;
    } else {
        if ( *rate < 4000 ) *rate = 4000;
        if ( *rate > sdev_caps_mono_maxrate ) *rate = sdev_caps_mono_maxrate;
    }
    adjustRate( rate, *stereo, &tc );
}

void sbSetDSPTimeConst( const uint8_t tc ) {
    sbioDSPWrite( sdev_hw_base, 0x40 );
    sbioDSPWrite( sdev_hw_base, tc );
}

void sbSetDSPFrequency( const uint16_t freq ) {
    sbioDSPWrite( sdev_hw_base, 0x41 );
    sbioDSPWrite( sdev_hw_base, freq >> 8 );
    sbioDSPWrite( sdev_hw_base, freq & 0xff );
}

void __far __pascal sbSetupMode( uint16_t freq, bool stereo ) {
    uint8_t tc;

    sbioDSPReset( sdev_hw_base );

    /* Calculate time constant and adjust rate
       For SB PRO we have to setup double samplerate in stereo mode */
    adjustRate( &freq, stereo, &tc );
    
    /* Set DSP time constant or frequency */
    if ( sbno == 6 )
        sbSetDSPFrequency( freq );
    else
        sbSetDSPTimeConst( tc );

    /* Setup stereo option for SB PRO
       For SB16 it's set in DSP command */
    if ( stereo & ( sbno != 6 ) )
        sbMixerWrite( 0x0e, sbMixerRead( 0x0e ) || 0x02 );

    /* Switch filter option off for SB PRO */
    if ( sbno == 2 || sbno == 4 || sbno == 5 )
        sbMixerWrite( 0x0e, sbMixerRead( 0x0e ) || 0x20 );

    sbSetSpeaker( true );
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
        /* DSP 0x48 - setup DMA buffer size */
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
