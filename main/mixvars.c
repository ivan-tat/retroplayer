/* mixvars.c -- mixer variables.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

/* TODO: remove EXTERN_LINK, PUBLIC_DATA and PUBLIC_CODE macros when done */

#include "..\pascal\pascal.h"

EXTERN_LINK uint16_t PUBLIC_DATA ST3Periods[12];
/* TODO: uncomment when done */
/*
uint16_t ST3Periods[12] = {
    1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907
};
*/

EXTERN_LINK uint16_t (*PUBLIC_DATA mixBuf)[];
EXTERN_LINK uint8_t  PUBLIC_DATA mixChannels;
EXTERN_LINK uint16_t PUBLIC_DATA mixSampleRate;
EXTERN_LINK uint16_t PUBLIC_DATA mixBufSamplesPerChannel;
EXTERN_LINK uint16_t PUBLIC_DATA mixBufSamples;
EXTERN_LINK uint16_t PUBLIC_DATA mixTickSamplesPerChannel;
EXTERN_LINK uint16_t PUBLIC_DATA mixTickSamplesPerChannelLeft;

void PUBLIC_CODE setMixChannels(uint8_t channels);
void PUBLIC_CODE setMixSampleRate(uint16_t rate);
void PUBLIC_CODE setMixBufSamplesPerChannel(uint16_t count);
void PUBLIC_CODE setMixMode(uint8_t channels, uint16_t rate, uint16_t count);

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
    unsigned int bufOff = count;
    if (mixChannels == 2) bufOff <<= 1;
    return bufOff << 1; /* int16_t (*buf)[]*/
};

uint16_t PUBLIC_CODE getCountFromMixBufOff(uint16_t bufOff)
{
    unsigned int count = bufOff;
    if (mixChannels == 2) count >>= 1;
    return count >> 1;  /* int16_t (*buf)[]*/
};
