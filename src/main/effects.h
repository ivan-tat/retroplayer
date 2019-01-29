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
#include "main/s3mvars.h"
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
#define EFFIDX_O_SAMPLE_OFFSET   15
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

typedef uint8_t row_state_flags_t;
typedef row_state_flags_t ROWSTATEFLAGS;

#define ROWSTATEFL_GLOBAL_VOLUME    (1 << 0)
#define ROWSTATEFL_JUMP_TO_ORDER    (1 << 1)
#define ROWSTATEFL_PATTERN_BREAK    (1 << 2)
#define ROWSTATEFL_PATTERN_LOOP     (1 << 3)
#define ROWSTATEFL_PATTERN_DELAY    (1 << 4)

#pragma pack(push, 1);
typedef struct row_state_t
{
    ROWSTATEFLAGS flags;
    uint8_t jump_pos;
    uint8_t break_pos;
    uint8_t global_volume;
};
#pragma pack(pop);
typedef struct row_state_t ROWSTATE;

/* Channel state */

typedef uint8_t channel_state_flags_t;
typedef channel_state_flags_t CHNSTATEFLAGS;

#define CHNSTATEFL_PORTAMENTO   (1 << 0)
#define CHNSTATEFL_ARPEGGIO     (1 << 1)

#pragma pack(push, 1);
typedef struct channel_state_t
{
    CHNSTATEFLAGS flags;
    // [Andre] normaly it will be a copie of ins, note, vol in
    // [Andre] pattern delay = 0 -> ignore note
    // [Andre] call 'readnotes' inside a pattern delay, if then ignore all inst/notes/vol !
    uint8_t cur_instrument;
    uint8_t cur_note;
    uint8_t cur_note_volume;
    // save effect, parameter for pattern delay
    uint8_t patdelay_saved_command;
    uint8_t patdelay_saved_parameter;
    // to save portamento values
    uint16_t porta_sample_period_old;
    uint32_t porta_sample_step_old;
};
#pragma pack(pop);
typedef struct channel_state_t CHNSTATE;

bool chn_effInit (PLAYSTATE *ps, ROWSTATE *rs, MIXCHN *chn, CHNSTATE *cs, uint8_t param);
void chn_effHandle (PLAYSTATE *ps, MIXCHN *chn, CHNSTATE *cs);
void chn_effTick (MUSMOD *track, PLAYSTATE *ps, MIXCHN *chn);
bool chn_effCanContinue (MIXCHN *chn, CHNSTATE *cs);
void chn_effStop (PLAYSTATE *ps, MIXCHN *chn);
void chn_effGetName(MIXCHN *chn, char *__s, size_t __maxlen);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux chn_effInit "*";
#pragma aux chn_effHandle "*";
#pragma aux chn_effTick "*";
#pragma aux chn_effCanContinue "*";
#pragma aux chn_effStop "*";
#pragma aux chn_effGetName "*";

#endif  /* __WATCOMC__ */

#endif  /* EFFECTS_H */
