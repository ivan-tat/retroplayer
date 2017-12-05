/* s3mvars.h -- declarations for s3mvars.pas.

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

// TODO: remove PUBLIC_DATA macros when done.

/* EMS */

extern bool PUBLIC_DATA UseEMS;
extern EMSHDL PUBLIC_DATA SavHandle;
    /* EM handle to save mapping while playing */

/* general module information */

extern bool             PUBLIC_DATA mod_isLoaded;
extern modTitle_t       PUBLIC_DATA mod_Title;          /* name given by the musician */
extern modTrackerName_t PUBLIC_DATA mod_TrackerName;    /* tracker version file was created with */

/* module options */

extern bool PUBLIC_DATA modOption_ST2Vibrato;   /* not supported */
extern bool PUBLIC_DATA modOption_ST2Tempo;     /* not supported */
extern bool PUBLIC_DATA modOption_AmigaSlides;  /* not supported */
extern bool PUBLIC_DATA modOption_SBfilter;     /* not supported */
extern bool PUBLIC_DATA modOption_CostumeFlag;  /* not supported */
extern bool PUBLIC_DATA modOption_VolZeroOptim; /* PSIs volume-zero optimization */
extern bool PUBLIC_DATA modOption_AmigaLimits;  /* check for amiga limits */
extern bool PUBLIC_DATA modOption_Stereo;

/* play options */

extern bool PUBLIC_DATA playOption_ST3Order;
    /* if true then handle order like ST3 - if a "--"=255 is found -
        stop or loop to the song start (look playOption_LoopSong) */
    /* if false - play the whole order and simply skip the "--"
        if (CurOrder==OrdNum) then stop or loop to the beginning */
extern bool PUBLIC_DATA playOption_LoopSong;
    /* flag if restart if we reach the end of the S3M module */

/* song arrangement */

extern ordersList_t PUBLIC_DATA Order;
extern uint16_t PUBLIC_DATA OrdNum;
extern uint8_t  PUBLIC_DATA LastOrder;  /* last order to play */

/* initial state */

extern uint8_t  PUBLIC_DATA initState_tempo;
extern uint8_t  PUBLIC_DATA initState_speed;
extern uint16_t PUBLIC_DATA initState_startOrder;

/* play state */

extern bool    PUBLIC_DATA playState_songEnded;
extern uint8_t PUBLIC_DATA playState_tempo;
extern uint8_t PUBLIC_DATA playState_speed;
extern uint8_t PUBLIC_DATA playState_gVolume;
extern uint8_t PUBLIC_DATA playState_mVolume;

/* position in song - you can change it while playing to jump arround */

extern uint8_t PUBLIC_DATA playState_order;
extern uint8_t PUBLIC_DATA playState_pattern;
extern uint8_t PUBLIC_DATA playState_row;
extern uint8_t PUBLIC_DATA playState_tick;

/* pattern loop */

extern bool    PUBLIC_DATA playState_patLoopActive;
extern uint8_t PUBLIC_DATA playState_patLoopCount;
extern uint8_t PUBLIC_DATA playState_patLoopStartRow;

/* pattern delay */

extern uint8_t PUBLIC_DATA playState_patDelayCount;

#endif  /* S3MVARS_H */
