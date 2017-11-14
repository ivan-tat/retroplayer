/* posttab.c -- functions to handle amplify table.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

#include "main/posttab.h"

static int16_t amptab[2][256];

void PUBLIC_CODE calcPostTable(uint8_t volume)
{
    int16_t v, i;

    v = volume & 127;

    for (i = 0; i < 256; i++)
    {
        amptab[0][i] = i * v / 128;
        amptab[1][i] = (int16_t)(((int32_t)((int16_t)((int8_t)i) * 256) * v) / 128);
    }
}

void amplify_16s(void *buf, uint16_t count)
{
    int16_t *out;
    int16_t s;

    if (count)
    {
        out = (int16_t *)buf;

        do
        {
            s = *out;
            *out = (amptab[0][(uint8_t)s] + amptab[1][(uint16_t)s >> 8]);
            out++;
            count--;
        }
        while (count);
    }
}

void clip_16s_8u(void *outbuf, void *mixbuf, uint16_t count)
{
    int16_t *src;
    uint8_t *dst;
    int16_t s;

    src = (int16_t *)mixbuf;
    dst = (uint8_t *)outbuf;

    do
    {
        s = *src;
        if (s < -128)
            s = -128;
        else
            if (s > 127)
                s = 127;
        *dst = (uint8_t)s + 128;
        src++;
        dst++;
        count--;
    }
    while (count);
}

void clip_16s_mono_8u_mono_lq(void *outbuf, void *mixbuf, uint16_t count)
{
    int16_t *src;
    uint8_t (*dst)[1];
    int16_t s;

    src = (int16_t *)mixbuf;
    dst = (uint8_t *)outbuf;

    do
    {
        s = *src;
        if (s < -128)
            s = -128;
        else
            if (s > 127)
                s = 127;
        *dst[0] = s;
        *dst[1] = s;
        src++;
        dst += 2;
        count--;
    }
    while (count);
}

void clip_16s_stereo_8u_stereo_lq(void *outbuf, void *mixbuf, uint16_t count)
{
    int16_t (*src)[1];
    uint8_t (*dst)[1];
    int16_t s[2];

    src = (int16_t *)mixbuf;
    dst = (uint8_t *)outbuf;

    count >>= 1;
    do
    {
        s[0] = *src[0];
        s[1] = *src[1];
        if (s[0] < -128)
            s[0] = -128;
        else
            if (s[0] > 127)
                s[0] = 127;
        if (s[1] < -128)
            s[1] = -128;
        else
            if (s[1] > 127)
                s[1] = 127;
        *dst[0] = s[0];
        *dst[1] = s[1];
        *dst[2] = s[0];
        *dst[3] = s[1];
        src += 2;
        dst += 4;
        count--;
    }
    while (count);
}
