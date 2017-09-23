/* mixer_.h -- declarations for mixer_.asm.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXER__H
#define MIXER__H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "main/mixtypes.h"

// TODO: remove PUBLIC_CODE macros when done.

extern void PUBLIC_CODE _MixSampleMono8(void *outBuf, struct playSampleInfo_t *smpInfo, uint16_t volTab, uint8_t vol, uint16_t count);
extern void PUBLIC_CODE _MixSampleStereo8(void *outBuf, struct playSampleInfo_t *smpInfo, uint16_t volTab, uint8_t vol, uint16_t count);

#endif  /* MIXER__H */
