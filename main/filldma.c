/* filldma.c -- some functions to fill DMA buffer.

   This is free and unencumbered software released into the public domain. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#endif

#include "..\dos\emstool.h"
#include "..\blaster\sbctl.h"
#include "posttab.h"

/* DMA buffer */

extern uint16_t __pascal DMArealbufsize[64];
extern void    *__pascal DMAbuffer;
extern uint8_t  __pascal DMAhalf;   /* last part of DMAbuffer we have to fill */
extern uint8_t  __pascal lastready; /* last part of DMAbuffer we calculated last call */
extern uint8_t  __pascal NumBuffers;

/* mixer */

extern void *__pascal TICKBUFFER;
extern bool  __pascal LQmode;
extern bool  __pascal toslow;
extern bool  __pascal JustInFill;

/* play state */

extern bool __pascal EndOfSong; /* Flag if we reach the end of the Song :( */

/* EMM */

extern bool     __pascal useEMS;
extern uint16_t __pascal savHandle;

/* other */

extern void __near __pascal calc_mono_tick( void );
extern void __near __pascal calc_stereo_tick( void );

/* locals */

static bool errorsav = false;

// public
void __near __pascal mixroutines( void ) {
    if ( stereo ) {
        __asm "push ds";    // FIXME
        calc_stereo_tick();
        __asm "pop ds";     // FIXME
    } else {
        __asm "push ds";    // FIXME
        calc_mono_tick();
        __asm "pop ds";     // FIXME
    }
}

void __near convert_8( void *outbuf, void *mixbuf, uint16_t count ) {
    uint16_t *src;
    uint8_t *dst;

    src = ( uint16_t* )mixbuf;
    dst = ( uint8_t *)outbuf;

    do {
        *dst = post8bit[ *src ];
        src++;
        dst++;
        count--;
    } while ( count );
}

void __near LQconvert_8( void *outbuf, void *mixbuf, uint16_t count ) {
    uint16_t ( *src )[1];
    uint8_t ( *dst )[1];
    uint8_t samp[2];

    src = ( uint16_t* )mixbuf;
    dst = ( uint8_t* )outbuf;

    if ( stereo ) {
        count >>= 1;
        do {
            samp[0] = post8bit[ *src[0] ];
            samp[1] = post8bit[ *src[1] ];
            *dst[0] = samp[0];
            *dst[1] = samp[1];
            *dst[2] = samp[0];
            *dst[3] = samp[1];
            src += 2;
            dst += 4;
            count--;
        } while ( count );
    } else {
        do {
            samp[0] = post8bit[ *src[0] ];
            *dst[0] = samp[0];
            *dst[1] = samp[0];
            src++;
            dst += 2;
            count--;
        } while ( count );
    }
}

void __near fill_8bit( void ) {
    uint16_t wait, bufsize, srcoff, dstoff;
    uint8_t ( *buf )[1];

    /* check if we are allready in calculation routines
       if we are the PC is too slow -> how you wanna handle it ? */

    buf = DMAbuffer;
    bufsize = DMArealbufsize[1]; // we have to calc. DMArealbufsize bytes

    if ( JustInFill ) {
        wait = 0xffff;
        while ( wait && JustInFill ) {
            wait--;
        }
        if ( JustInFill ) {
            /* sorry your PC is to slow - maincode may ignore this flag */
            /* but it'll sound ugly :( */
            toslow = true;

            /* simply fill the half with last correct mixed value */
            dstoff = DMArealbufsize[ DMAhalf ];
            DMAhalf = 1-DMAhalf;
            srcoff = DMArealbufsize[ DMAhalf ];
            // FIXME: 8-bits stereo is two byte fill, not one
            memset( &( buf[ dstoff ] ), *buf[ srcoff+bufsize-1 ], bufsize );
            return;
        }
    }
    /* for check if too slow set a variable (flag that we are allready in calc) */

    JustInFill = true;

    if ( EndOfSong ) {
        // clear DMAbuffer
        memset( &( buf[ DMArealbufsize[ DMAhalf ] ] ), 0, bufsize );
        DMAhalf = 1-DMAhalf;
    } else {
        // before calling mixroutines: save EMM mapping !
        if ( useEMS ) {
            errorsav = true;
            if ( EmsSaveMap( savHandle ) ) errorsav = false;
        }

        mixroutines(); // calc 'DMArealbufsize' bytes into the TICKBUFFER

        // now restore EMM mapping:
        if ( useEMS ) {
            if ( ! errorsav ) EmsRestoreMap( savHandle );
        }

        lastready = ( lastready+1 ) & ( NumBuffers-1 );
        dstoff = DMArealbufsize[ lastready ];

        if ( LQmode )
            LQconvert_8( &( buf[ dstoff<<1 ] ), TICKBUFFER, bufsize );
        else
            convert_8( &( buf[ dstoff ] ), TICKBUFFER, bufsize );
    }

    JustInFill = false;
}

// public
void __near __pascal fill_DMAbuffer( void ) {
    if ( ! _16bit ) {
        do {
            fill_8bit();
        } while ( lastready != DMAhalf );
    }
}
