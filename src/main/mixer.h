/* mixer.h -- declarations for mixer.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXER_H
#define MIXER_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

// TODO: remove PUBLIC_CODE macros when done.

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

extern uint16_t PUBLIC_DATA ST3Periods[12];

#define getNotePeriod(note) ((ST3Periods[(note) & 0x0f] << 4) >> ((note) >> 4))

extern uint32_t PUBLIC_CODE _calc_sample_step(uint16_t wPeriod);

#ifdef __WATCOMC__
#pragma aux _calc_sample_step modify [ bx cx ];
#endif

extern uint16_t (*PUBLIC_DATA mixBuf)[];
    /* Mixing buffer for one frame.
       Size depends on: sample rate, channels count, tempo, FPS (almost as DMA frame) */
extern uint16_t PUBLIC_DATA mixSampleRate;
extern uint8_t  PUBLIC_DATA mixChannels;
extern uint16_t PUBLIC_DATA mixBufSamplesPerChannel;
extern uint16_t PUBLIC_DATA mixBufSamples;
extern uint16_t PUBLIC_DATA mixTickSamplesPerChannel;
    /* Samples per channel per tick - depends on sample rate and tempo */
extern uint16_t PUBLIC_DATA mixTickSamplesPerChannelLeft;
    /* Samples per channel left to next tick */

extern void     PUBLIC_CODE setMixChannels(uint8_t channels);
extern void     PUBLIC_CODE setMixSampleRate(uint16_t rate);
extern void     PUBLIC_CODE setMixBufSamplesPerChannel(uint16_t count);
extern void     PUBLIC_CODE setMixMode(uint8_t channels, uint16_t rate, uint16_t count);
extern uint16_t PUBLIC_CODE getMixBufOffFromCount(uint16_t count);
extern uint16_t PUBLIC_CODE getCountFromMixBufOff(uint16_t bufOff);

#endif /* MIXER_H */
