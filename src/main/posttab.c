/* posttab.c -- functions to handle amplify table.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

#include "main/posttab.h"

static uint8_t  PUBLIC_DATA post8bit[4096];
static uint16_t PUBLIC_DATA post16bit[4096];

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

void convert_16s_8u(void *outbuf, void *mixbuf, uint16_t count)
{
    uint16_t *src;
    uint8_t *dst;

    src = (uint16_t *)mixbuf;
    dst = (uint8_t *)outbuf;

    do
    {
        *dst = post8bit[*src + 2048];
        src++;
        dst++;
        count--;
    }
    while (count);
}

void convert_16s_mono_8u_mono_lq(void *outbuf, void *mixbuf, uint16_t count)
{
    uint16_t (*src)[1];
    uint8_t (*dst)[1];
    uint8_t samp[2];

    src = (uint16_t *)mixbuf;
    dst = (uint8_t *)outbuf;

    do
    {
        samp[0] = post8bit[*src[0] + 2048];
        *dst[0] = samp[0];
        *dst[1] = samp[0];
        src++;
        dst += 2;
        count--;
    }
    while (count);
}

void convert_16s_stereo_8u_stereo_lq(void *outbuf, void *mixbuf, uint16_t count)
{
    uint16_t (*src)[1];
    uint8_t (*dst)[1];
    uint8_t samp[2];

    src = (uint16_t *)mixbuf;
    dst = (uint8_t *)outbuf;

    count >>= 1;
    do
    {
        samp[0] = post8bit[*src[0] + 2048];
        samp[1] = post8bit[*src[1] + 2048];
        *dst[0] = samp[0];
        *dst[1] = samp[1];
        *dst[2] = samp[0];
        *dst[3] = samp[1];
        src += 2;
        dst += 4;
        count--;
    }
    while (count);
}
