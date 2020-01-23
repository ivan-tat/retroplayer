/* mixer.c -- mixer functions.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$main$mixer$*"
#endif  /* __WATCOMC__ */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "common.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/dos.h"
#include "debug.h"
#include "main/s3mtypes.h"
#include "main/mixer.h"

uint32_t _calc_sample_step(uint16_t period, uint16_t rate)
{
    long long int a = (long long int) MID_C_PERIOD * MID_C_RATE << 16;
    long int b = (long int)period * (long int)rate;
    b = a / b;
    return b;
}

#if DEFINE_LOCAL_DATA == 1

uint16_t ST3Periods[12] =
{
#include "main/nperiods.inc"
};

#endif  /* DEFINE_LOCAL_DATA == 1 */

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

/*** Mixer ***/

MIXERFLAGS __mixer_set_flags (MIXERFLAGS _flags, MIXERFLAGS _mask, MIXERFLAGS _set, bool raise)
{
    if (raise)
        return (_flags & _mask) | _set;
    else
        return _flags & _mask;
}

void mixer_init (MIXER *self)
{
    memset (self, 0, sizeof (MIXER));
}

bool __near _mixer_alloc_smpbuf (MIXER *self)
{
    SMPBUF *smpbuf;

    smpbuf = _new (SMPBUF);
    if (!smpbuf)
        return false;

    _mixer_set_smpbuf (self, smpbuf);
    _mixer_set_own_smpbuf (self, true);

    if (!smpbuf_alloc (smpbuf, _mixer_get_num_spc (self)))
        return false;

    return true;
}

void __near _mixer_free_smpbuf (MIXER *self)
{
    SMPBUF *smpbuf;

    smpbuf = _mixer_get_smpbuf (self);
    if (smpbuf)
    {
        if (_mixer_is_own_smpbuf (self))
        {
            smpbuf_free (smpbuf);
            _delete (smpbuf);
            _mixer_set_own_smpbuf (self, false);
        }
        _mixer_set_smpbuf (self, NULL);
    }
}

bool __near _mixer_alloc_mixbuf (MIXER *self)
{
    MIXBUF *mixbuf;

    mixbuf = _new (MIXBUF);
    if (!mixbuf)
        return false;

    _mixer_set_mixbuf (self, mixbuf);
    _mixer_set_own_mixbuf (self, true);

    if (!mixbuf_alloc (mixbuf, _mixer_get_num_channels (self) * _mixer_get_num_spc (self)))
        return false;

    return true;
}

void __near _mixer_free_mixbuf (MIXER *self)
{
    MIXBUF *mixbuf;

    mixbuf = _mixer_get_mixbuf (self);
    if (mixbuf)
    {
        if (_mixer_is_own_mixbuf (self))
        {
            mixbuf_free (mixbuf);
            _delete (mixbuf);
            _mixer_set_own_mixbuf (self, false);
        }
        _mixer_set_mixbuf (self, NULL);
    }
}

bool mixer_alloc_buffers (MIXER *self, MIXERBUFMASK mask)
{
    if (mask & MIXERBUFMASK_SMPBUF)
    {
        _mixer_free_smpbuf (self);
        if (!_mixer_alloc_smpbuf (self))
            return false;
    }

    if (mask & MIXERBUFMASK_MIXBUF)
    {
        _mixer_free_mixbuf (self);
        if (!_mixer_alloc_mixbuf (self))
            return false;
    }

    return true;
}

void mixer_free_buffers (MIXER *self, MIXERBUFMASK mask)
{
    if (mask & MIXERBUFMASK_SMPBUF)
        _mixer_free_smpbuf (self);

    if (mask & MIXERBUFMASK_MIXBUF)
        _mixer_free_mixbuf (self);
}

void mixer_free (MIXER *self)
{
    _mixer_free_smpbuf (self);
    _mixer_free_mixbuf (self);
}
