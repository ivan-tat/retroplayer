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

// TODO: remove PUBLIC_CODE macros when done.

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

extern void PUBLIC_CODE _MixSampleMono8(
    void *outBuf,
    struct playSampleInfo_t *smpInfo,
    uint16_t volTab,
    uint8_t vol,
    uint16_t count
);

extern void PUBLIC_CODE _MixSampleStereo8(
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

extern uint16_t PUBLIC_DATA ST3Periods[12];

#define getNotePeriod(note) ((ST3Periods[(note) & 0x0f] << 4) >> ((note) >> 4))

uint32_t _calc_sample_step(uint16_t period, uint16_t rate);

/* Mixing buffer for one frame.
   Size depends on: sample rate, channels count, tempo, FPS (almost as DMA frame) */

#pragma pack(push, 1);
typedef struct mixing_buffer_t
{
    int16_t *buf;
    uint16_t size;
    uint8_t channels;
    uint16_t samples_per_channel;
};
#pragma pack(pop);
typedef struct mixing_buffer_t MIXBUF;

void     mixbuf_init(MIXBUF *self);
bool     mixbuf_alloc(MIXBUF *self, uint16_t size);
void     mixbuf_set_channels(MIXBUF *self, uint8_t value);
void     mixbuf_set_samples_per_channel(MIXBUF *self, uint16_t value);
void     mixbuf_set_mode(MIXBUF *self, uint8_t channels, uint16_t samples_per_channel);
uint16_t mixbuf_get_offset_from_count(MIXBUF *self, uint16_t value);
uint16_t mixbuf_get_count_from_offset(MIXBUF *self, uint16_t value);
void     mixbuf_free(MIXBUF *self);

/* Variables */

extern MIXBUF mixBuf;

/* Linking */

#ifdef __WATCOMC__
#pragma aux _MixSampleMono8 "*";
#pragma aux _MixSampleMono16 "*";
#pragma aux fill_8 "*";
#pragma aux fill_16 "*";
#pragma aux fill_32 "*";
#pragma aux _calc_sample_step "*";
#pragma aux mixBuf "*";
#pragma aux mixbuf_init "*";
#pragma aux mixbuf_alloc "*";
#pragma aux mixbuf_set_channels "*";
#pragma aux mixbuf_set_samples_per_channel "*";
#pragma aux mixbuf_set_mode "*";
#pragma aux mixbuf_get_offset_from_count "*";
#pragma aux mixbuf_get_count_from_offset "*";
#pragma aux mixbuf_free "*";
#endif

#endif /* MIXER_H */
