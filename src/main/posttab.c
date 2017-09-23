/* posttab.c -- functions to handle amplify table.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

#include "main/posttab.h"

void PUBLIC_CODE calcPostTable(uint8_t vol, bool use16bit)
{
    int16_t z, i, sample;

    if ( ! use16bit )
    {
        z = vol&127;
        for( i = 0; i <= 4095; i++ )
        {
            sample = ( int16_t )( 127 + ( ( ( int32_t )( i - 2048 ) * z ) >> 7 ) );
            if( sample < 0 ) sample = 0; else
            if( sample > 255 ) sample = 255;
            post8bit[ i ] = sample;
        }
    }
}
