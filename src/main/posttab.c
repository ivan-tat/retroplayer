/* posttab.c -- functions to handle amplify table.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$main$posttab$*"
#endif

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#if DEBUG == 1
# include "cc/stdio.h"
#endif
#include "main/posttab.h"

#pragma pack(push, 1)
typedef union sample_32_t
{
    uint8_t u8[4];
    int8_t s8[4];
    uint16_t u16[2];
    int16_t s16[2];
    uint32_t u32;
    int32_t s32;
};
#pragma pack(pop);

static int32_t amptab[3][0x100];

#define MID_VOL (1 << 7)

#define CLIP_MIN -0x8000L
#define CLIP_MAX 0x7fffL

void amptab_set_volume(uint8_t volume)
{
    int16_t vol;
    int32_t *p, s, d;
    int16_t i;
    #if DEBUG == 1
    FILE *f;
    #endif

    vol = volume;
    if (vol > MID_VOL)
        vol = MID_VOL;

    p = amptab;

    // i = sample & 0xff
    // s0[i] = (u32)((u8)i) * vol / MID_VOL
    s = 0;
    d = vol;
    for (i = 0; i < 0x100; i++)
    {
        *p = s / MID_VOL;
        p++;
    }

    // i = (sample & 0xff00) >> 8
    // s1[i] = ((u32)(((u8)s_in)) << 8) * vol / MID_VOL
    s = 0;
    //d = (vol << 8) / MID_VOL;
    d <<= 1;
    for (i = 0; i < 0x100; i++)
    {
        *p = s;
        s += d;
        p++;
    }

    // i = (sample & 0xff0000) >> 16
    // s2[i] = ((s32)(((s8)i)) << 16) * vol / MID_VOL
    s = 0;
    //d = (vol << 16) / MID_VOL;
    d <<= 8;
    for (i = 0; i < 0x80; i++)
    {
        *p = s;
        s += d;
        p++;
    }

    s = -(((int32_t)vol) << 16);
    for (i = 0; i < 0x80; i++)
    {
        *p = s;
        s += d;
        p++;
    }

    #if DEBUG == 1
    f = fopen ("_amp0", "wb+");
    if (f)
    {
        fwrite (&amptab[0], 256*4, 1, f);
        fclose (f);
    }
    f = fopen ("_amp1", "wb+");
    if (f)
    {
        fwrite (&amptab[1], 256*4, 1, f);
        fclose (f);
    }
    f = fopen ("_amp2", "wb+");
    if (f)
    {
        fwrite (&amptab[2], 256*4, 1, f);
        fclose (f);
    }
    #endif  /* DEBUG */
}

// "mb" is mixing buffer

void amplify_s32(int32_t *mb, uint16_t count)
{
    union sample_32_t *out;

    out = (union sample_32_t *)mb;

    while (count)
    {
        out->s32 = amptab[0][out->u8[0]] + amptab[1][out->u8[1]] + amptab[2][out->u8[2]];
        out++;
        count--;
    }
}

void clip_s32_u8(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s;
    uint8_t *out;

    src = mb;
    out = (uint8_t *)outbuf;

    while (count)
    {
        s = *src;
        if (s < CLIP_MIN)
            s = CLIP_MIN;
        else
            if (s > CLIP_MAX)
                s = CLIP_MAX;

        *out = (uint8_t)(((uint16_t)s & 0xff00) >> 8) ^ 0x80;

        src++;
        out++;
        count--;
    }
}

void clip_s32_s8(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s;
    int8_t *out;

    src = mb;
    out = (int8_t *)outbuf;

    while (count)
    {
        s = *src;
        if (s < CLIP_MIN)
            s = CLIP_MIN;
        else
            if (s > CLIP_MAX)
                s = CLIP_MAX;

        *out = (int8_t)(((uint16_t)s & 0xff00) >> 8);

        src++;
        out++;
        count--;
    }
}

void clip_s32_u16(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s;
    uint16_t *out;

    src = mb;
    out = (uint16_t *)outbuf;

    while (count)
    {
        s = *src;
        if (s < CLIP_MIN)
            s = CLIP_MIN;
        else
            if (s > CLIP_MAX)
                s = CLIP_MAX;

        *out = (uint16_t)s ^ 0x8000;

        src++;
        out++;
        count--;
    }
}

void clip_s32_s16(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s;
    int16_t *out;

    src = mb;
    out = (int16_t *)outbuf;

    while (count)
    {
        s = *src;
        if (s < CLIP_MIN)
            s = CLIP_MIN;
        else
            if (s > CLIP_MAX)
                s = CLIP_MAX;

        *out = (int16_t)s;

        src++;
        out++;
        count--;
    }
}

void clip_s32_u8_lq(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s;
    uint16_t *out;

    src = mb;
    out = (uint16_t *)outbuf;

    while (count)
    {
        s = src[0];

        if (s < CLIP_MIN)
            s = CLIP_MIN;
        else
            if (s > CLIP_MAX)
                s = CLIP_MAX;

        *out = (((uint16_t)s & 0xff00) + (((uint16_t)s & 0xff00) > 8)) ^ 0x8080;

        src++;
        out++;
        count--;
    }
}

void clip_s32_s8_lq(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s;
    uint16_t *out;

    src = mb;
    out = (uint16_t *)outbuf;

    while (count)
    {
        s = src[0];

        if (s < CLIP_MIN)
            s = CLIP_MIN;
        else
            if (s > CLIP_MAX)
                s = CLIP_MAX;

        *out = ((uint16_t)s & 0xff00) + (((uint16_t)s & 0xff00) > 8);

        src++;
        out++;
        count--;
    }
}

void clip_s32_u16_lq(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s;
    uint16_t *out;

    src = mb;
    out = (uint16_t *)outbuf;

    while (count)
    {
        s = src[0];

        if (s < CLIP_MIN)
            s = CLIP_MIN;
        else
            if (s > CLIP_MAX)
                s = CLIP_MAX;

        s ^= 0x8000;
        out[0] = (uint16_t)s;
        out[1] = (uint16_t)s;

        src++;
        out += 2;
        count--;
    }
}

void clip_s32_s16_lq(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s;
    int16_t *out;

    src = mb;
    out = (int16_t *)outbuf;

    while (count)
    {
        s = src[0];

        if (s < CLIP_MIN)
            s = CLIP_MIN;
        else
            if (s > CLIP_MAX)
                s = CLIP_MAX;

        out[0] = (int16_t)s;
        out[1] = (int16_t)s;

        src++;
        out += 2;
        count--;
    }
}

void clip_s32_u8_lq_stereo(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s[2];
    uint16_t *out, v;

    src = mb;
    out = (uint16_t *)outbuf;

    while (count)
    {
        s[0] = src[0];
        s[1] = src[1];

        if (s[0] < CLIP_MIN)
            s[0] = CLIP_MIN;
        else
            if (s[0] > CLIP_MAX)
                s[0] = CLIP_MAX;

        if (s[1] < CLIP_MIN)
            s[1] = CLIP_MIN;
        else
            if (s[1] > CLIP_MAX)
                s[1] = CLIP_MAX;

        v = ((((uint16_t)s[0] & 0xff00) >> 8) + ((uint16_t)s[1] & 0xff00)) ^ 0x8080;

        out[0] = v;
        out[1] = v;

        src += 2;
        out += 2;
        count--;
    }
}

void clip_s32_s8_lq_stereo(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s[2];
    uint16_t *out, v;

    src = mb;
    out = (uint16_t *)outbuf;

    while (count)
    {
        s[0] = src[0];
        s[1] = src[1];

        if (s[0] < CLIP_MIN)
            s[0] = CLIP_MIN;
        else
            if (s[0] > CLIP_MAX)
                s[0] = CLIP_MAX;

        if (s[1] < CLIP_MIN)
            s[1] = CLIP_MIN;
        else
            if (s[1] > CLIP_MAX)
                s[1] = CLIP_MAX;

        v = (((uint16_t)s[0] & 0xff00) >> 8) + ((uint16_t)s[1] & 0xff00);

        out[0] = v;
        out[1] = v;

        src += 2;
        out += 2;
        count--;
    }
}

void clip_s32_u16_lq_stereo(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s[2];
    uint32_t *out;
    union sample_32_t v;

    src = mb;
    out = (uint32_t *)outbuf;

    while (count)
    {
        s[0] = src[0];
        s[1] = src[1];

        if (s[0] < CLIP_MIN)
            s[0] = CLIP_MIN;
        else
            if (s[0] > CLIP_MAX)
                s[0] = CLIP_MAX;

        if (s[1] < CLIP_MIN)
            s[1] = CLIP_MIN;
        else
            if (s[1] > CLIP_MAX)
                s[1] = CLIP_MAX;

        v.u16[0] = (uint16_t)s[0];
        v.u16[1] = (uint16_t)s[1];
        v.u32 ^= 0x80008000L;

        out[0] = v.u32;
        out[1] = v.u32;

        src += 2;
        out += 2;
        count--;
    }
}

void clip_s32_s16_lq_stereo(void *outbuf, int32_t *mb, uint16_t count)
{
    int32_t *src, s[2];
    uint32_t *out;
    union sample_32_t v;

    src = mb;
    out = (uint32_t *)outbuf;

    while (count)
    {
        s[0] = src[0];
        s[1] = src[1];

        if (s[0] < CLIP_MIN)
            s[0] = CLIP_MIN;
        else
            if (s[0] > CLIP_MAX)
                s[0] = CLIP_MAX;

        if (s[1] < CLIP_MIN)
            s[1] = CLIP_MIN;
        else
            if (s[1] > CLIP_MAX)
                s[1] = CLIP_MAX;

        v.s16[0] = (int16_t)s[0];
        v.s16[1] = (int16_t)s[1];
        v.u32 ^= 0x80008000L;

        out[0] = v.u32;
        out[1] = v.u32;

        src += 2;
        out += 2;
        count--;
    }
}
