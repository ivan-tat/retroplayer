/* mixer.h -- declarations for mixer.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXER_H
#define MIXER_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"

/* Sample playing */

#define MIXSMPFL_16BITS 0x01
#define MIXSMPFL_LOOP   0x02

#pragma pack(push, 1);
typedef struct playSampleInfo_t
{
    void    *dData;
    uint32_t dPos;
    uint32_t dStep;
    uint16_t wLen;
    uint16_t wLoopStart;
    uint16_t wLoopEnd;
    uint16_t wFlags;
};
#pragma pack(pop);

extern void __far __pascal _play_sample_nearest_8 (
    void *outbuf,
    struct playSampleInfo_t *smpInfo,
    uint16_t count
);

extern void __far __pascal _play_sample_nearest_16 (
    void *outbuf,
    struct playSampleInfo_t *smpInfo,
    uint16_t count
);

extern void __far __pascal _mix_sample (
    void *outbuf,
    void *smpbuf,
    uint16_t voltab,
    uint8_t vol,
    uint16_t count
);

extern void __far __pascal _mix_sample2 (
    void *outbuf,
    void *smpbuf,
    uint16_t voltab,
    uint8_t vol,
    uint16_t count
);

extern void __far __pascal _MixSampleMono8 (
    void *outBuf,
    struct playSampleInfo_t *smpInfo,
    uint16_t volTab,
    uint8_t vol,
    uint16_t count
);

extern void __far __pascal _MixSampleMono16 (
    void *outBuf,
    struct playSampleInfo_t *smpInfo,
    uint16_t volTab,
    uint8_t vol,
    uint16_t count
);

extern void __far __pascal _MixSampleStereo8 (
    void *outBuf,
    struct playSampleInfo_t *smpInfo,
    uint16_t volTab,
    uint8_t vol,
    uint16_t count
);

extern void __far __pascal _MixSampleStereo16 (
    void *outBuf,
    struct playSampleInfo_t *smpInfo,
    uint16_t volTab,
    uint8_t vol,
    uint16_t count
);

/* Filling */

void fill_8(void *dest, uint8_t value, uint16_t count);
void fill_16(void *dest, uint16_t value, uint16_t count);
void fill_32(void *dest, uint32_t value, uint16_t count);

/* Playing */

extern uint16_t ST3Periods[12];

#define getNotePeriod(note) ((ST3Periods[(note) & 0x0f] << 4) >> ((note) >> 4))

uint32_t _calc_sample_step(uint16_t period, uint16_t rate);

/* Sample buffer for at least one frame */

#pragma pack(push, 1);
typedef struct sample_buffer_t
{
    int16_t *buf;
    uint16_t len;
};
#pragma pack(pop);
typedef struct sample_buffer_t SMPBUF;

void     smpbuf_init(SMPBUF *self);
bool     smpbuf_alloc(SMPBUF *self, uint16_t len);
int16_t *smpbuf_get(SMPBUF *self);
uint16_t smpbuf_get_length(SMPBUF *self);
void     smpbuf_free(SMPBUF *self);

/* Mixing buffer for at least one frame.
   Size depends on: sample rate, channels count, tempo, FPS (almost as DMA frame) */

#pragma pack(push, 1);
typedef struct mixing_buffer_t
{
    int32_t *buf;
    uint16_t len;
    uint8_t channels;
    uint16_t samples_per_channel;
};
#pragma pack(pop);
typedef struct mixing_buffer_t MIXBUF;

void     mixbuf_init(MIXBUF *self);
bool     mixbuf_alloc(MIXBUF *self, uint16_t len);
int32_t *mixbuf_get(MIXBUF *self);
uint16_t mixbuf_get_length(MIXBUF *self);
void     mixbuf_set_channels(MIXBUF *self, uint8_t value);
uint8_t  mixbuf_get_channels(MIXBUF *self);
void     mixbuf_set_samples_per_channel(MIXBUF *self, uint16_t value);
uint16_t mixbuf_get_samples_per_channel(MIXBUF *self);
void     mixbuf_set_mode(MIXBUF *self, uint8_t channels, uint16_t samples_per_channel);
uint16_t mixbuf_get_offset_from_count(MIXBUF *self, uint16_t value);
uint16_t mixbuf_get_count_from_offset(MIXBUF *self, uint16_t value);
void     mixbuf_free(MIXBUF *self);

/*** Mixer ***/

typedef uint8_t mixer_flags_t;
typedef mixer_flags_t MIXERFLAGS;

#define MIXERFL_OWN_SMPBUF  (1 << 0)    // has own sample buffer, free it when done
#define MIXERFL_OWN_MIXBUF  (1 << 1)    // has own mixing buffer, free it when done

typedef uint8_t mixer_quality_t;
typedef mixer_quality_t MIXERQUALITY;

#define MIXQ_NEAREST    0
#define MIXQ_FASTEST    1
#define MIXQ_MAX        1

typedef uint8_t mixer_buffers_mask_t;
typedef mixer_buffers_mask_t MIXERBUFMASK;

#define MIXERBUFMASK_SMPBUF   (1 << 0)
#define MIXERBUFMASK_MIXBUF   (1 << 1)

#pragma pack(push, 1);
typedef struct mixer_t
{
    MIXERFLAGS flags;
    MIXERQUALITY quality;
    uint8_t num_channels;
    uint16_t num_spc;   // samples per channel
    SMPBUF *smpbuf;
    MIXBUF *mixbuf;
};
#pragma pack(pop);
typedef struct mixer_t MIXER;

MIXERFLAGS __mixer_set_flags (MIXERFLAGS _flags, MIXERFLAGS _mask, MIXERFLAGS _set, bool raise);

#define _mixer_get_flags(o)             (o)->flags
#define _mixer_set_flags(o, v)          _mixer_get_flags (o) = v
#define _mixer_is_own_smpbuf(o)         ((_mixer_get_flags (o) & MIXERBUFMASK_SMPBUF) != 0)
#define _mixer_set_own_smpbuf(o, v)     _mixer_set_flags (o, __mixer_set_flags (_mixer_get_flags (o), ~MIXERFL_OWN_SMPBUF, MIXERFL_OWN_SMPBUF, v))
#define _mixer_is_own_mixbuf(o)         ((_mixer_get_flags (o) & MIXERBUFMASK_MIXBUF) != 0)
#define _mixer_set_own_mixbuf(o, v)     _mixer_set_flags (o, __mixer_set_flags (_mixer_get_flags (o), ~MIXERFL_OWN_MIXBUF, MIXERFL_OWN_MIXBUF, v))
#define _mixer_get_quality(o)           (o)->quality
#define _mixer_set_quality(o, v)        _mixer_get_quality (o) = v
#define _mixer_get_num_channels(o)      (o)->num_channels
#define _mixer_set_num_channels(o, v)   _mixer_get_num_channels (o) = v
#define _mixer_get_num_spc(o)           (o)->num_spc
#define _mixer_set_num_spc(o, v)        _mixer_get_num_spc (o) = v
#define _mixer_get_smpbuf(o)            (o)->smpbuf
#define _mixer_set_smpbuf(o, v)         _mixer_get_smpbuf (o) = v
#define _mixer_get_mixbuf(o)            (o)->mixbuf
#define _mixer_set_mixbuf(o, v)         _mixer_get_mixbuf (o) = v

void    mixer_init (MIXER *self);
#define mixer_get_flags(o)              _mixer_get_flags(o)
#define mixer_set_flags(o, v)           _mixer_set_flags(o, v)
#define mixer_is_own_smpbuf(o)          _mixer_is_own_smpbuf (o)
#define mixer_set_own_smpbuf(o, v)      _mixer_set_own_smpbuf (o, v)
#define mixer_is_own_mixbuf(o)          _mixer_is_own_mixbuf (o)
#define mixer_set_own_mixbuf(o, v)      _mixer_set_own_mixbuf (o, v)
#define mixer_get_quality(o)            _mixer_get_quality (o)
#define mixer_set_quality(o, v)         _mixer_set_quality (o, v)
#define mixer_get_num_channels(o)       _mixer_get_num_channels (o)
#define mixer_set_num_channels(o, v)    _mixer_set_num_channels (o, v)
#define mixer_get_num_spc(o)            _mixer_get_num_spc (o)
#define mixer_set_num_spc(o, v)         _mixer_set_num_spc (o, v)
#define mixer_get_smpbuf(o)             _mixer_get_smpbuf (o)
#define mixer_set_smpbuf(o, v)          _mixer_set_smpbuf (o, v)
#define mixer_get_mixbuf(o)             _mixer_get_mixbuf (o)
#define mixer_set_mixbuf(o, v)          _mixer_set_mixbuf (o, v)
bool    mixer_alloc_buffers (MIXER *self, MIXERBUFMASK mask);
void    mixer_free_buffers (MIXER *self, MIXERBUFMASK mask);
void    mixer_free (MIXER *self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux ST3Periods "*";

#pragma aux _play_sample_nearest_8 "^";
#pragma aux _play_sample_nearest_16 "^";

#pragma aux _mix_sample "^";
#pragma aux _mix_sample2 "^";

#pragma aux _MixSampleMono8 "^";
#pragma aux _MixSampleMono16 "^";
#pragma aux _MixSampleStereo8 "^";
#pragma aux _MixSampleStereo16 "^";

#pragma aux fill_8 "*";
#pragma aux fill_16 "*";
#pragma aux fill_32 "*";

#pragma aux _calc_sample_step "*";

#pragma aux smpbuf_init "*";
#pragma aux smpbuf_alloc "*";
#pragma aux smpbuf_get "*";
#pragma aux smpbuf_get_length "*";
#pragma aux smpbuf_free "*";

#pragma aux mixbuf_init "*";
#pragma aux mixbuf_alloc "*";
#pragma aux mixbuf_get "*";
#pragma aux mixbuf_get_length "*";
#pragma aux mixbuf_set_channels "*";
#pragma aux mixbuf_get_channels "*";
#pragma aux mixbuf_set_samples_per_channel "*";
#pragma aux mixbuf_get_samples_per_channel "*";
#pragma aux mixbuf_set_mode "*";
#pragma aux mixbuf_get_offset_from_count "*";
#pragma aux mixbuf_get_count_from_offset "*";
#pragma aux mixbuf_free "*";

#pragma aux __mixer_set_flags "*";
#pragma aux mixer_init "*";
#pragma aux mixer_alloc_buffers "*";
#pragma aux mixer_free_buffers "*";
#pragma aux mixer_free "*";

#endif  /* __WATCOMC__ */

#endif /* MIXER_H */
