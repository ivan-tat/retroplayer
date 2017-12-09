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

/* TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done */

#ifdef DEFINE_LOCAL_DATA

static uint16_t ST3Periods[12] =
{
    1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907
};

static MIXBUF mixBuf;

#endif

void mixbuf_init(MIXBUF *self)
{
    if (self)
    {
        self->buf = NULL;
        self->channels = 0;
        self->samples_per_channel = 0;
    }
}

bool mixbuf_alloc(MIXBUF *self, uint16_t size)
{
    uint16_t seg;

    if (self)
    {
        if (!_dos_allocmem(_dos_para(size), &seg))
        {
            self->buf = MK_FP(seg, 0);
            self->size = size;
            return true;
        }
    }

    return false;
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

void mixbuf_set_channels(MIXBUF *self, uint8_t value)
{
    self->channels = value;
    mixbuf_set_samples_per_channel(self, self->samples_per_channel);
}

void mixbuf_set_samples_per_channel(MIXBUF *self, uint16_t value)
{
    self->samples_per_channel = value;
}

void mixbuf_set_mode(MIXBUF *self, uint8_t channels, uint16_t samples_per_channel)
{
    self->channels = channels;
    mixbuf_set_samples_per_channel(self, samples_per_channel);
}

uint16_t mixbuf_get_offset_from_count(MIXBUF *self, uint16_t value)
{
    uint16_t result;

    if (self)
    {
        result = value;

        if (self->channels == 2)
            result <<= 1;

        return result * sizeof(int16_t);    // (16 bits)
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

        return result / sizeof(int16_t);    // (16 bits)
    }

    return 0;
}
