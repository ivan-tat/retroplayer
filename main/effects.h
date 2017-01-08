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

#define EFFIDX_NONE              0
#define EFFIDX_A_SET_SPEED       1
#define EFFIDX_B_JUMP            2
#define EFFIDX_C_PATTERN_BREAK   3
#define EFFIDX_D_VOLUME_SLIDE    4
#define EFFIDX_E_PITCH_DOWN      5
#define EFFIDX_F_PITCH_UP        6
#define EFFIDX_G_PORTAMENTO      7
#define EFFIDX_H_VIBRATO         8
#define EFFIDX_I_TREMOR          9
#define EFFIDX_J_ARPEGGIO        10
#define EFFIDX_K_VIB_VOLSLIDE    11
#define EFFIDX_L_PORTA_VOLSLIDE  12
#define EFFIDX_M                 13
#define EFFIDX_N                 14
#define EFFIDX_O                 15
#define EFFIDX_P                 16
#define EFFIDX_Q_RETRIG_VOLSLIDE 17
#define EFFIDX_R_TREMOLO         18
#define EFFIDX_S_SPECIAL         19
#define EFFIDX_T_SET_TEMPO       20
#define EFFIDX_U_FINE_VIBRATO    21
#define EFFIDX_V_SET_GVOLUME     22
#define EFFIDX_W                 23
#define EFFIDX_X                 24
#define EFFIDX_Y                 25
#define EFFIDX_Z                 26

void PUBLIC_CODE set_tempo(uint8_t tempo);

/* TODO: rename to 'void *patMapData(MUSPAT *pat)', and move to s3mvars.c */
void *PUBLIC_CODE mapPatternData(void *pat);

void PUBLIC_CODE chn_setSamplePeriod(MIXCHN *chn, int32_t period);
void PUBLIC_CODE chn_setSampleVolume(MIXCHN *chn, int16_t vol);
void PUBLIC_CODE chn_setupInstrument(MIXCHN *chn, uint8_t insNum);

/* remove if unused outside effects.c */
uint16_t PUBLIC_CODE chn_calcNotePeriod(MIXCHN *chn, struct instrument_t *ins, uint8_t note);
uint32_t PUBLIC_CODE chn_calcNoteStep(MIXCHN *chn, struct instrument_t *ins, uint8_t note);

void PUBLIC_CODE chn_setupNote(MIXCHN *chn, uint8_t note, bool keep);

bool PUBLIC_CODE chn_effInit(MIXCHN *chn, uint8_t param);
void PUBLIC_CODE chn_effHandle(MIXCHN *chn);
void PUBLIC_CODE chn_effTick(MIXCHN *chn);

#endif  /* EFFECTS_H */
