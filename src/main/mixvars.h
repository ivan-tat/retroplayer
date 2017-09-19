/* mixvars.h -- declarations for mixvars.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXVARS_H
#define MIXVARS_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "pascal/pascal.h"

extern uint16_t PUBLIC_DATA ST3Periods[12];

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

#define getNotePeriod(note) ((ST3Periods[(note) & 0x0f] << 4) >> ((note) >> 4))

#endif  /* MIXVARS_H */
