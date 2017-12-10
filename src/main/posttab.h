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

void amptab_set_volume(uint8_t vol);

void amplify_s32(int32_t *buf, uint16_t count);
void clip_s32_u8(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_s8(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_u16(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_s16(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_u8_lq(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_s8_lq(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_u16_lq(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_s16_lq(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_u8_lq_stereo(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_s8_lq_stereo(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_u16_lq_stereo(void *outbuf, int32_t *mixbuf, uint16_t count);
void clip_s32_s16_lq_stereo(void *outbuf, int32_t *mixbuf, uint16_t count);

#ifdef __WATCOMC__
#pragma aux amptab_set_volume "*";
#pragma aux amplify_s32 "*";
#pragma aux clip_s32_u8 "*";
#pragma aux clip_s32_s8 "*";
#pragma aux clip_s32_u16 "*";
#pragma aux clip_s32_s16 "*";
#pragma aux clip_s32_u8_lq "*";
#pragma aux clip_s32_s8_lq "*";
#pragma aux clip_s32_u16_lq "*";
#pragma aux clip_s32_s16_lq "*";
#pragma aux clip_s32_u8_lq_stereo "*";
#pragma aux clip_s32_s8_lq_stereo "*";
#pragma aux clip_s32_u16_lq_stereo "*";
#pragma aux clip_s32_s16_lq_stereo "*";
#endif

#endif  /* POSTTAB_H */
