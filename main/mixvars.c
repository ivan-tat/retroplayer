/* mixvars.c -- mixer variables.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

uint16_t ST3Periods[12] = {
    1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907
};

uint16_t UseRate;
uint16_t mixTickSamplesPerChannel;
uint16_t mixTickSamplesPerChannelLeft;
