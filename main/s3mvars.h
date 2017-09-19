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

#include "../pascal/pascal.h"
#include "s3mtypes.h"

/* EMM */

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

/* patterns */

extern patternsList_t PUBLIC_DATA Pattern;
extern uint16_t PUBLIC_DATA patListCount;
extern uint16_t PUBLIC_DATA patListPatLength;   /* length of one pattern */
extern bool     PUBLIC_DATA patListUseEM;       /* patterns in EM */
extern uint16_t PUBLIC_DATA patListEMHandle;    /* handle to access EM for patterns */
extern uint8_t  PUBLIC_DATA patListPatPerEMPage;  /* count of patterns per page (<64!!!) */

#define isPatternDataInEM(seg)                ((seg) >= 0xc000)
#define getPatternDataLogPageInEM(seg)        ((seg) & 0x00ff)
#define getPatternDataPartInEM(seg)           (((seg) >> 8) & 0x3f)
#define getPatternDataOffsetInEM(seg, length) (getPatternDataPartInEM(seg) * (length))
#define setPatternDataInEM(logpage, part)     (0xc000 + (((part) & 0x3f) << 8) + (logpage))
#define getPatternDataInEM(seg, length)       (MK_FP(FrameSEG[0], getPatternDataOffsetInEM((seg), (length))))

//MUSPAT *PUBLIC_CODE pat_new(void);
void    PUBLIC_CODE pat_clear(MUSPAT *pat);
//void    PUBLIC_CODE pat_delete(MUSPAT **pat);
void    PUBLIC_CODE patSetData(MUSPAT *pat, void *p);
void    PUBLIC_CODE patSetDataInEM(MUSPAT *pat, uint8_t logpage, uint8_t part);
bool    PUBLIC_CODE patIsDataInEM(MUSPAT *pat);
void   *PUBLIC_CODE patGetData(MUSPAT *pat);
uint8_t PUBLIC_CODE patGetDataLogPageInEM(MUSPAT *pat);
uint8_t PUBLIC_CODE patGetDataPartInEM(MUSPAT *pat);
void   *PUBLIC_CODE patMapData(MUSPAT *pat);
void    PUBLIC_CODE patFree(MUSPAT *pat);
//void    PUBLIC_CODE patInit(MUSPAT *pat);
//void    PUBLIC_CODE patDone(MUSPAT *pat);

void     PUBLIC_CODE patList_set(int16_t index, MUSPAT *pat);
MUSPAT  *PUBLIC_CODE patList_get(int16_t index);
uint32_t PUBLIC_CODE patListGetUsedEM(void);
void     PUBLIC_CODE patListFree(void);
void     PUBLIC_CODE patListInit(void);
void     PUBLIC_CODE patListDone(void);

/* song arrangement */

extern ordersList_t PUBLIC_DATA Order;
extern uint16_t PUBLIC_DATA OrdNum;
extern uint8_t  PUBLIC_DATA LastOrder;  /* last order to play */

#ifdef DEBUG
extern uint16_t PUBLIC_DATA StartOrder;
#endif

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

#endif  /* S3MVARS_H */
