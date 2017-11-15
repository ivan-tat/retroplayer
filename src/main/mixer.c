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

uint32_t PUBLIC_CODE _calc_sample_step(uint16_t wPeriod)
{
    long long int a = (long long int) 1712 * 8363 << 16;
    long int b = (long int)wPeriod * (long int)mixSampleRate;
    b = a / b;
    return b;
}

/* TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done */

#ifdef DEFINE_LOCAL_DATA

uint16_t ST3Periods[12] =
{
    1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907
};

MIXBUF   PUBLIC_DATA mixBuf;
uint8_t  PUBLIC_DATA mixChannels;
uint16_t PUBLIC_DATA mixSampleRate;
uint16_t PUBLIC_DATA mixBufSamplesPerChannel;
uint16_t PUBLIC_DATA mixBufSamples;
uint16_t PUBLIC_DATA mixTickSamplesPerChannel;
uint16_t PUBLIC_DATA mixTickSamplesPerChannelLeft;

#endif

void PUBLIC_CODE setMixChannels(uint8_t channels);
void PUBLIC_CODE setMixSampleRate(uint16_t rate);
void PUBLIC_CODE setMixBufSamplesPerChannel(uint16_t count);
void PUBLIC_CODE setMixMode(uint8_t channels, uint16_t rate, uint16_t count);

void PUBLIC_CODE mixbuf_init(MIXBUF *self)
{
    if (self)
    {
        self->buf = NULL;
        mixSampleRate = 0;
        mixChannels = 0;
        mixBufSamplesPerChannel = 0;
    }
}

bool PUBLIC_CODE mixbuf_alloc(MIXBUF *self, uint16_t size)
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

void PUBLIC_CODE mixbuf_free(MIXBUF *self)
{
    if (self)
        if (self->buf)
        {
            _dos_freemem(FP_SEG(self->buf));
            self->buf = NULL;
        }
}

void PUBLIC_CODE setMixChannels(uint8_t channels)
{
    mixChannels = channels;
    setMixBufSamplesPerChannel(mixBufSamplesPerChannel);
}

void PUBLIC_CODE setMixSampleRate(uint16_t rate)
{
    mixSampleRate = rate;
}

void PUBLIC_CODE setMixBufSamplesPerChannel(uint16_t count)
{
    mixBufSamplesPerChannel = count;
    mixBufSamples = mixChannels * count;
}

void PUBLIC_CODE setMixMode(uint8_t channels, uint16_t rate, uint16_t count)
{
    mixChannels = channels;
    mixSampleRate = rate;
    setMixBufSamplesPerChannel(count);
}

uint16_t PUBLIC_CODE getMixBufOffFromCount(uint16_t count)
{
    uint16_t bufOff;

    bufOff = count;

    if (mixChannels == 2)
        bufOff <<= 1;

    return bufOff << 1; /* int16_t (*buf)[]*/
}

uint16_t PUBLIC_CODE getCountFromMixBufOff(uint16_t bufOff)
{
    uint16_t count;

    count = bufOff;
    if (mixChannels == 2)
        count >>= 1;

    return count >> 1;  /* int16_t (*buf)[]*/
}
