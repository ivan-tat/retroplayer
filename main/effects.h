/* effects.h -- declarations for effects.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef EFFECTS_H
#define EFFECTS_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove reserved words "extern", PUBLIC_CODE and PUBLIC_DATA macros when done.

#include "..\pascal\pascal.h"

#include "s3mtypes.h"

void PUBLIC_CODE chn_setSamplePeriod(struct channel_t *chn, int32_t period);
void PUBLIC_CODE chn_setSampleVolume(struct channel_t *chn, int16_t vol);
void PUBLIC_CODE chn_setupInstrument(struct channel_t *chn, uint8_t insNum);
void PUBLIC_CODE chn_setupNote(struct channel_t *chn, uint8_t note, bool keep);
void PUBLIC_CODE chn_effTick(struct channel_t *chn);

#endif  /* EFFECTS_H */
