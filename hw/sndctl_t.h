/* sndctl_t.h -- type declarations for sndctl.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef SNDCTL_T_H
#define SNDCTL_T_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "../pascal/pascal.h"

/* Hardware sample format */

typedef uint8_t HWSMPFMTFLAGS;

#define HWSMPFMTFL_BITS_MASK 0x7f
#define HWSMPFMTFL_SIGNED    0x80

typedef struct hwSampleFormat_t {
    HWSMPFMTFLAGS flags;
    uint8_t channels;
};
typedef struct hwSampleFormat_t HWSMPFMT;

bool     PUBLIC_CODE set_sample_format(HWSMPFMT *p, uint8_t bits, bool sign, uint8_t channels);
uint8_t  PUBLIC_CODE get_sample_format_bits(HWSMPFMT *p);
bool     PUBLIC_CODE is_sample_format_signed(HWSMPFMT *p);
uint8_t  PUBLIC_CODE get_sample_format_channels(HWSMPFMT *p);
uint16_t PUBLIC_CODE get_sample_format_width(HWSMPFMT *p);
void     PUBLIC_CODE clear_sample_format(HWSMPFMT *p);

#endif  /* SNDCTL_T_H */
