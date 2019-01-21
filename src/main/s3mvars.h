/* s3mvars.h -- declarations for s3mvars.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef S3MVARS_H
#define S3MVARS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "dos/ems.h"
#include "main/s3mtypes.h"

#ifdef DEBUG_WRITE_OUTPUT_STREAM
// write sound output streams to files (mixing buffer and DMA buffer)
#include "cc/stdio.h"
extern FILE *_debug_stream[2];
#endif  /* DEBUG_WRITE_OUTPUT_STREAM */

/* EMS */

extern bool UseEMS;
extern EMSHDL SavHandle;
    /* EM handle to save mapping while playing */

/* play options */

extern bool playOption_ST3Order;
    /* if true then handle order like ST3 - if a "--"=255 is found -
        stop or loop to the song start (look playOption_LoopSong) */
    /* if false - play the whole order and simply skip the "--"
        if (CurOrder==order_length) then stop or loop to the beginning */
extern bool playOption_LoopSong;
    /* flag if restart if we reach the end of the S3M module */

/* song arrangement */

extern uint8_t  LastOrder;  /* last order to play */

/* initial state */

extern uint16_t initState_startOrder;

/*** play state ***/

typedef uint8_t play_state_flags_t;
typedef play_state_flags_t PLAYSTATEFLAGS;

#define PLAYSTATEFL_END     (1 << 0)    // end of song
#define PLAYSTATEFL_PATLOOP (1 << 1)    // pattern loop

#pragma pack(push, 1);
typedef struct playState_t
{
    PLAYSTATEFLAGS flags;
    uint16_t rate;
    uint8_t  tempo;
    uint8_t  speed;
    uint8_t  global_volume;
    uint8_t  master_volume;
    uint16_t tick_samples_per_channel;  // depends on rate and tempo
    // position in song - you can change it while playing to jump arround
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
typedef struct playState_t PLAYSTATE;

extern PLAYSTATE playState;

void __far playState_set_speed (PLAYSTATE *self, uint8_t value);
void __far playState_set_tempo (PLAYSTATE *self, uint8_t value);

/*** Linking ***/

#ifdef __WATCOMC__

#ifdef DEBUG_WRITE_OUTPUT_STREAM
#pragma aux _debug_stream "*";
#endif

#pragma aux UseEMS "*";
#pragma aux SavHandle "*";

#pragma aux playOption_ST3Order "*";
#pragma aux playOption_LoopSong "*";

#pragma aux LastOrder "*";

#pragma aux initState_startOrder "*";

#pragma aux playState "*";

#pragma aux playState_set_speed "*";
#pragma aux playState_set_tempo "*";

#endif  /* __WATCOMC__ */

#endif  /* S3MVARS_H */
