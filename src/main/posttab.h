/* posttab.h -- declarations for posttab.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef POSTTAB_H
#define POSTTAB_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

void PUBLIC_CODE calcPostTable(uint8_t vol);

void amplify_16s(void *buf, uint16_t count);
void clip_16s_8u(void *outbuf, void *mixbuf, uint16_t count);
void clip_16s_mono_8u_mono_lq(void *outbuf, void *mixbuf, uint16_t count);
void clip_16s_stereo_8u_stereo_lq(void *outbuf, void *mixbuf, uint16_t count);

/* Set name mangling to none for editing purposes */
#ifdef __WATCOMC__
#pragma aux amplify_16s "*";
#pragma aux clip_16s_8u "*";
#pragma aux clip_16s_mono_8u_mono_lq "*";
#pragma aux clip_16s_stereo_8u_stereo_lq "*";
#endif

#endif  /* POSTTAB_H */
