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
#include "main/s3mtypes.h"

// TODO: remove PUBLIC_DATA macros when done.

/* EMS */

extern bool PUBLIC_DATA UseEMS;

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
extern bool     PUBLIC_DATA EMSSmp;         /* samples in EMS ? */
extern uint16_t PUBLIC_DATA SmpEMSHandle;   /* hanlde to access EMS for samples */

//#define insList_get(num) (Instruments[(num)-1])
//#define insList_get(num) (struct instrument_t *)(MK_FP(FP_SEG(*Instruments), FP_OFF(*Instruments) + ((num) - 1) * sizeof(struct instrument_t)))
#define insList_get(num) (struct instrument_t *)MK_FP(FP_SEG(Instruments[0]) + ((num) - 1) * 5, 0)
#define ins_isSample(ins) (ins->bType == 1)
#define ins_isSampleLooped(ins) (ins->flags & 0x01 != 0)
#define ins_getSampleLoopStart(ins) (ins->loopbeg)
#define ins_getSampleLoopEnd(ins) (ins->flags & 0x01 ? ins->loopend : ins->slength)
#define ins_getSampleRate(ins) (ins->c2speed)
#define ins_getSampleData(ins) (void *)MK_FP(ins->memseg, 0)

#define isSampleDataInEM(seg) (seg >= 0xf000)
#define getSampleDataLogPageInEM(seg) (seg & 0x0fff)

/* song arrangement */

extern ordersList_t PUBLIC_DATA Order;
extern uint16_t PUBLIC_DATA OrdNum;
extern uint8_t  PUBLIC_DATA LastOrder;  /* last order to play */

/* channels */

extern channelsList_t PUBLIC_DATA Channel;  /* all public/private data for every channel */
extern uint8_t PUBLIC_DATA UsedChannels;    /* possible values : 1..32 (kill all Adlib) */

#define chn_getState(chn)        (chn->bEnabled)
#define chn_setState(chn, value) chn->bEnabled = value

#define _chn_setSamplePeriod(chn, value) chn->wSmpPeriod= value
#define chn_getSamplePeriod(chn)         (chn->wSmpPeriod)

#define chn_getSampleStep(chn)          (chn->dSmpStep)
#define chn_setSampleStep(chn, value)   chn->dSmpStep = value

#define chn_getInstrument(chn) (struct instrument_t *)MK_FP(chn->wInsSeg, 0)
#define chn_setInstrument(chn, p) chn->wInsSeg = FP_SEG((void __far *)p)
#define chn_setSampleData(chn, p) chn->wSmpSeg = FP_SEG((void __far *)p)

#define chn_setCommand(chn, value)     chn->bCommand = value
#define chn_getCommand(chn)            (chn->bCommand)
#define chn_setSubCommand(chn, value)  chn->bCommand2 = value
#define chn_getSubCommand(chn)         (chn->bCommand2)
#define chn_setEffectParam(chn, value) chn->bParameter = value
#define chn_getEffectParam(chn)        (chn->bParameter)

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
