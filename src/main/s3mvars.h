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

/* play state */

extern bool     playState_songEnded;
extern uint16_t playState_rate;
extern uint8_t  playState_tempo;
extern uint8_t  playState_speed;
extern uint8_t  playState_gVolume;
extern uint8_t  playState_mVolume;
extern uint16_t playState_tick_samples_per_channel;

/* position in song - you can change it while playing to jump arround */

extern uint8_t  playState_order;
extern uint8_t  playState_pattern;
extern uint8_t  playState_row;
extern uint8_t  playState_tick;
extern uint16_t playState_tick_samples_per_channel_left;

/* pattern loop */

extern bool    playState_patLoopActive;
extern uint8_t playState_patLoopCount;
extern uint8_t playState_patLoopStartRow;

/* pattern delay */

extern uint8_t playState_patDelayCount;

void playState_set_speed(uint8_t value);
void playState_set_tempo(uint8_t value);

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

#pragma aux playState_songEnded "*";
#pragma aux playState_rate "*";
#pragma aux playState_tempo "*";
#pragma aux playState_speed "*";
#pragma aux playState_gVolume "*";
#pragma aux playState_mVolume "*";
#pragma aux playState_tick_samples_per_channel "*";

#pragma aux playState_order "*";
#pragma aux playState_pattern "*";
#pragma aux playState_row "*";
#pragma aux playState_tick "*";
#pragma aux playState_tick_samples_per_channel_left "*";

#pragma aux playState_patLoopActive "*";
#pragma aux playState_patLoopCount "*";
#pragma aux playState_patLoopStartRow "*";

#pragma aux playState_patDelayCount "*";

#pragma aux playState_set_speed "*";
#pragma aux playState_set_tempo "*";

#endif  /* __WATCOMC__ */

#endif  /* S3MVARS_H */
