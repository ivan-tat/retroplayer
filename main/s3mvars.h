/* s3mvars.h -- declarations for s3mvars.pas.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef S3MVARS_H
#define S3MVARS_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_DATA macros when done.

#include "..\pascal\pascal.h"

#include "s3mtypes.h"

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
extern bool PUBLIC_DATA modOption_SignedData;   /* signed/unsigned data */
                                                /* (only volumetable differs in those modes) */
extern bool PUBLIC_DATA modOption_Stereo;

/* play options */

extern bool PUBLIC_DATA playOption_ST3Order;
    /* if true then handle order like ST3 - if a "--"=255 is found -
        stop or loop to the song start (look playOption_LoopSong) */
    /* if false - play the whole order and simply skip the "--"
        if (CurOrder==OrdNum) then stop or loop to the beginning */
extern bool PUBLIC_DATA playOption_LoopSong;
    /* flag if restart if we reach the end of the S3M module */

/* instruments */

extern instrumentsList_t *PUBLIC_DATA Instruments;  /* pointer to data for all instruments */
extern uint16_t PUBLIC_DATA InsNum;

/* patterns */

extern patternsList_t PUBLIC_DATA Pattern;  /* segment for every pattern */
                                            /* 0xFxyy -> at EMS page YY on Offset X*5120 */
extern uint16_t PUBLIC_DATA PatNum;
extern uint16_t PUBLIC_DATA PatLength;      /* length of one pattern */

/* song arrangement */

extern ordersList_t PUBLIC_DATA Order;
extern uint16_t PUBLIC_DATA OrdNum;
extern uint8_t  PUBLIC_DATA LastOrder;  /* last order to play */

#ifdef BETATEST
extern uint16_t PUBLIC_DATA StartOrder;
#endif

extern channelsList_t PUBLIC_DATA Channel;  /* all public/private data for every channel */
extern uint8_t PUBLIC_DATA UsedChannels;    /* possible values : 1..32 (kill all Adlib) */

/* initial state */

extern uint8_t PUBLIC_DATA InitTempo;   /* initial tempo */
extern uint8_t PUBLIC_DATA InitSpeed;   /* initial speed */

/* play state */

extern bool    PUBLIC_DATA EndOfSong;
extern uint8_t PUBLIC_DATA CurTempo;    /* current tempo - count of ticks per note */
extern uint8_t PUBLIC_DATA CurSpeed;    /* current speed - length of one tick */
extern uint8_t PUBLIC_DATA GVolume;     /* global volume -> usedvol = instrvol*gvolume/255 */
extern uint8_t PUBLIC_DATA MVolume;     /* master volume -> calc posttables */

/* position in song - you can change it while playing to jump arround */

extern uint8_t PUBLIC_DATA CurOrder;    /* position in song arrangement */
extern uint8_t PUBLIC_DATA CurPattern;  /* current pattern - is specified also by Order[curorder] */
                                        /* so it's only for the user ... */
extern uint8_t PUBLIC_DATA CurLine;     /* current line in pattern */
extern uint8_t PUBLIC_DATA CurTick;     /* current tick - we only calc one tick per call */
                                        /* (look at MIXING.ASM) */

/* pattern loop */

extern bool    PUBLIC_DATA PLoop_On;    /* in a Pattern loop? */
extern uint8_t PUBLIC_DATA PLoop_No;    /* number of loops left */
extern uint8_t PUBLIC_DATA PLoop_To;    /* position to loop to */

/* pattern delay */

extern uint8_t PUBLIC_DATA PatternDelay;

/* EMM */

extern bool     PUBLIC_DATA UseEMS;
extern uint16_t PUBLIC_DATA PatEMSHandle;   /* handle to access EMS for patterns */
extern uint16_t PUBLIC_DATA SmpEMSHandle;   /* hanlde to access EMS for samples */
                                            /* I seperated them, but that does not matter, well ? */
extern bool     PUBLIC_DATA EMSPat;         /* patterns in EMS ? */
extern bool     PUBLIC_DATA EMSSmp;         /* samples in EMS ? */
extern uint8_t  PUBLIC_DATA PatPerPage;     /* count of patterns per page (<64!!!) */

#endif /* S3MVARS_H */
