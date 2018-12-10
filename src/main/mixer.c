/* mixer.c -- mixer functions.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "debug.h"

#include "main/mixer.h"

uint32_t _calc_sample_step(uint16_t period, uint16_t rate)
{
    long long int a = (long long int) 1712 * 8363 << 16;
    long int b = (long int)period * (long int)rate;
    b = a / b;
    return b;
}

#ifdef DEFINE_LOCAL_DATA

static uint16_t ST3Periods[12] =
{
    1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907
};

static MIXBUF mixBuf;

#endif

/* Filling */

void fill_8(void *dest, uint8_t value, uint16_t count)
{
    memset(dest, value, count);
}

void fill_16(void *dest, uint16_t value, uint16_t count)
{
    register uint16_t *p;
    register uint16_t n;

    p = dest;
    while (n)
    {
        *p = value;
        p++;
        n--;
    }
}

void fill_32(void *dest, uint32_t value, uint16_t count)
{
    register uint32_t *p;
    register uint16_t n;

    p = dest;
    while (n)
    {
        *p = value;
        p++;
        n--;
    }
}

/* Sample buffer */

void smpbuf_init(SMPBUF *self)
{
    if (self)
    {
        self->buf = NULL;
        self->len = 0;
    }
}

bool smpbuf_alloc(SMPBUF *self, uint16_t len)
{
    uint16_t seg;

    if (self)
    {
        if (!_dos_allocmem(_dos_para(len * sizeof(int16_t)), &seg))
        {
            self->buf = MK_FP(seg, 0);
            self->len = len;
            return true;
        }
    }

    return false;
}

int16_t *smpbuf_get(SMPBUF *self)
{
    if (self)
        return self->buf;

    return NULL;
}

uint16_t smpbuf_get_length(SMPBUF *self)
{
    if (self)
        return self->len;

    return 0;
}

void smpbuf_free(SMPBUF *self)
{
    if (self)
        if (self->buf)
        {
            _dos_freemem(FP_SEG(self->buf));
            self->buf = NULL;
        }
}

/* Mixing buffer */

void mixbuf_init(MIXBUF *self)
{
    if (self)
    {
        self->buf = NULL;
        self->len = 0;
        self->channels = 0;
        self->samples_per_channel = 0;
    }
}

bool mixbuf_alloc(MIXBUF *self, uint16_t len)
{
    uint16_t seg;

    if (self)
    {
        if (!_dos_allocmem(_dos_para(len * sizeof(int32_t)), &seg))
        {
            self->buf = MK_FP(seg, 0);
            self->len = len;
            return true;
        }
    }

    return false;
}

int32_t *mixbuf_get(MIXBUF *self)
{
    if (self)
        return self->buf;

    return NULL;
}

uint16_t mixbuf_get_length(MIXBUF *self)
{
    if (self)
        return self->len;

    return 0;
}

void mixbuf_set_channels(MIXBUF *self, uint8_t value)
{
    if (self)
        self->channels = value;
}

uint8_t mixbuf_get_channels(MIXBUF *self)
{
    if (self)
        return self->channels;

    return 0;
}

void mixbuf_set_samples_per_channel(MIXBUF *self, uint16_t value)
{
    if (self)
        self->samples_per_channel = value;
}

uint16_t mixbuf_get_samples_per_channel(MIXBUF *self)
{
    if (self)
        return self->samples_per_channel;

    return 0;
}

void mixbuf_set_mode(MIXBUF *self, uint8_t channels, uint16_t samples_per_channel)
{
    if (self)
    {
        self->channels = channels;
        self->samples_per_channel = samples_per_channel;
    }
}

uint16_t mixbuf_get_offset_from_count(MIXBUF *self, uint16_t value)
{
    uint16_t result;

    if (self)
    {
        result = value;

        if (self->channels == 2)
            result <<= 1;

        return result * sizeof(int32_t);
    }

    return 0;
}

uint16_t mixbuf_get_count_from_offset(MIXBUF *self, uint16_t value)
{
    uint16_t result;

    if (self)
    {
        result = value;

        if (self->channels == 2)
            result >>= 1;

        return result / sizeof(int32_t);
    }

    return 0;
}

void mixbuf_free(MIXBUF *self)
{
    if (self)
        if (self->buf)
        {
            _dos_freemem(FP_SEG(self->buf));
            self->buf = NULL;
        }
}
