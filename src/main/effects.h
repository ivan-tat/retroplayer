/* effects.h -- declarations for effects.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef EFFECTS_H
#define EFFECTS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "main/musins.h"
#include "main/s3mtypes.h"
#include "main/mixchn.h"

#define EFFGROUP_EMPTY   0
#define EFFGROUP_GLOBAL  1
#define EFFGROUP_PATTERN 2
#define EFFGROUP_ROW     3
#define EFFGROUP_NOTE    4
#define EFFGROUP_PITCH   5
#define EFFGROUP_VOLUME  6
#define EFFGROUP_PANNING 7
#define EFFGROUP_SAMPLE  8

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

#define MAXEFF 22

/* Flow control variables */
// TODO: make portable: place it to a structure and pass pointer to all effect's methods
extern bool    playState_jumpToOrder_bFlag;
extern uint8_t playState_jumpToOrder_bPos;
extern bool    playState_patBreak_bFlag;
extern uint8_t playState_patBreak_bPos;
extern bool    playState_patLoop_bNow;
extern bool    playState_gVolume_bFlag;
extern uint8_t playState_gVolume_bValue;
extern bool    playState_patDelay_bNow;

/* Channel state */

/* [Andre] call 'readnotes' inside a pattern delay, */
/* [Andre] if then ignore all notes/inst/vol ! */

/* [Andre] now some variables I added after I found out those amazing */
/* [Andre] things about pattern delay */

/* save effect,parameter for pattern delay */
extern uint16_t chnState_patDelay_bCommandSaved;
extern uint8_t  chnState_patDelay_bParameterSaved;

/* [Andre] normaly it will be a copie of es:[di], but in */
/* [Andre] pattern delay = 0 -> ignore note */
extern uint8_t chnState_cur_bNote;
extern uint8_t chnState_cur_bIns;   /* the same thing for instrument */
extern uint8_t chnState_cur_bVol;   /* and for volume */

/* to save portamento values : */
extern bool     chnState_porta_flag;
extern uint16_t chnState_porta_wSmpPeriodOld;
extern uint32_t chnState_porta_dSmpStepOld;

extern bool     chnState_arp_bFlag;
    /* a little one for arpeggio */

bool chn_effInit(MIXCHN *chn, uint8_t param);
void chn_effHandle(MIXCHN *chn);
void chn_effTick(MIXCHN *chn);
bool chn_effCanContinue(MIXCHN *chn);
void chn_effStop(MIXCHN *chn);
void chn_effGetName(MIXCHN *chn, char *__s, size_t __maxlen);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux playState_jumpToOrder_bFlag "*";
#pragma aux playState_jumpToOrder_bPos "*";
#pragma aux playState_patBreak_bFlag "*";
#pragma aux playState_patBreak_bPos "*";
#pragma aux playState_patLoop_bNow "*";
#pragma aux playState_gVolume_bFlag "*";
#pragma aux playState_gVolume_bValue "*";
#pragma aux playState_patDelay_bNow "*";

#pragma aux chnState_patDelay_bCommandSaved "*";
#pragma aux chnState_patDelay_bParameterSaved "*";

#pragma aux chnState_cur_bNote "*";
#pragma aux chnState_cur_bIns "*";
#pragma aux chnState_cur_bVol "*";

#pragma aux chnState_porta_flag "*";
#pragma aux chnState_porta_wSmpPeriodOld "*";
#pragma aux chnState_porta_dSmpStepOld "*";

#pragma aux chnState_arp_bFlag "*";

#pragma aux chn_effInit "*";
#pragma aux chn_effHandle "*";
#pragma aux chn_effTick "*";
#pragma aux chn_effCanContinue "*";
#pragma aux chn_effStop "*";
#pragma aux chn_effGetName "*";

#endif  /* __WATCOMC__ */

#endif  /* EFFECTS_H */
