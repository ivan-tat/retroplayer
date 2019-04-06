/* musmodps.h -- declarations for musmodps.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MUSMODPS_H
#define MUSMODPS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "main/s3mtypes.h"
#include "main/musmod.h"
#include "main/mixchn.h"

/*** play state ***/

typedef uint8_t play_state_flags_t;
typedef play_state_flags_t PLAYSTATEFLAGS;

#define PLAYSTATEFL_END         (1 << 0)    // end of song
#define PLAYSTATEFL_PATLOOP     (1 << 1)    // pattern loop
#define PLAYSTATEFL_SONGLOOP    (1 << 2)    // song loop
#define PLAYSTATEFL_SKIPENDMARK (1 << 3)    // skip "end" mark in patterns' order

#pragma pack(push, 1);
typedef struct play_state_t
{
    MUSMOD *track;
    MIXCHNLIST *channels;
    PLAYSTATEFLAGS flags;
    uint16_t rate;
    uint8_t  tempo;
    uint8_t  speed;
    uint8_t  global_volume;
    uint8_t  master_volume;
    uint16_t tick_samples_per_channel;  // depends on rate and tempo
    // position in song - you can change it while playing to jump arround
    uint8_t  order_start;   // start position
    uint8_t  order_last;    // last order to play
    uint8_t  order;
    uint8_t  pattern;
    uint8_t  row;
    uint8_t  tick;
    uint16_t tick_samples_per_channel_left; // samples per channel left to next tick
    // pattern loop
    uint8_t  patloop_count;
    uint8_t  patloop_start_row;
    // pattern delay
    uint8_t  patdelay_count;
};
#pragma pack(pop);
typedef struct play_state_t PLAYSTATE;

void __far playstate_init (PLAYSTATE *self);
bool __far playstate_alloc_channels (PLAYSTATE *self);
void __far playstate_reset_channels (PLAYSTATE *self);
void __far playstate_free_channels (PLAYSTATE *self);
void __far playstate_set_speed (PLAYSTATE *self, uint8_t value);
void __far playstate_set_tempo (PLAYSTATE *self, uint8_t value);
void __far playstate_setup_patterns_order (PLAYSTATE *self);
int  __far playstate_find_next_pattern (PLAYSTATE *self, int index, int step);
void __far playstate_set_pos (PLAYSTATE *self, uint8_t start_order, uint8_t start_row, bool keep);
void __far playstate_set_initial_state (PLAYSTATE *self);
void __far playstate_free (PLAYSTATE *self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux playstate_init "*";
#pragma aux playstate_alloc_channels "*";
#pragma aux playstate_reset_channels "*";
#pragma aux playstate_free_channels "*";
#pragma aux playstate_set_speed "*";
#pragma aux playstate_set_tempo "*";
#pragma aux playstate_setup_patterns_order "*";
#pragma aux playstate_find_next_pattern "*";
#pragma aux playstate_set_pos "*";
#pragma aux playstate_set_initial_state "*";
#pragma aux playstate_free "*";

#endif  /* __WATCOMC__ */

#endif  /* MUSMODPS_H */
