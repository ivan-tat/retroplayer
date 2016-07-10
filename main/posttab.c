#include "posttab.h"

void __far __pascal calcPostTable( uint8_t vol, bool use16bit )
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
