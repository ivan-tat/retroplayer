/* musins.h -- declarations for musical instrument handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MUSINS_H
#define MUSINS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "dos/ems.h"
#include "dynarray.h"

/*** Musical instrument ***/

typedef uint8_t music_instrument_type_t;
typedef music_instrument_type_t MUSINSTYPE;

#define MUSINST_EMPTY 0
#define MUSINST_PCM   1
#define MUSINST_ADLIB 2

#define SMPFLAG_LOOP 0x01

#pragma pack(push, 1);
typedef struct instrument_t
{
    MUSINSTYPE bType;
    uint8_t  DOSname[12];
    uint8_t  unused1;
    uint16_t memseg;        // segment of sampledata
    uint16_t slength;       // length of sample <= 64KB
    uint16_t HI_length;     // <- not used !
    uint16_t loopbeg;       // loop begin <= 64KB
    uint16_t HI_loopbeg;    // <- not used !
    uint16_t loopend;       // loop end <= 64KB
    uint16_t HI_loopend;    // <- not used !
    uint8_t  vol;           // default instrument volume
    uint8_t  unused2;
    uint8_t  packinfo;      // =0 or we can't play it !
    uint8_t  flags;         // bit 0: loop (all other bits are ignored)
    uint16_t c2speed;       // frequency at middle C (actually C-4)
    uint16_t c2speed_hi;    // ignored
    uint8_t  unused3[12];
    uint8_t  IName[28];
    uint32_t SCRS_ID;
};
#pragma pack(pop);
typedef struct instrument_t MUSINS;

typedef struct instrument_t PCMSMP;

#define ins_isSample(ins) (ins->bType == 1)
#define ins_isSampleLooped(ins) (ins->flags & 0x01 != 0)
#define ins_getSampleLoopStart(ins) (ins->loopbeg)
#define ins_getSampleLoopEnd(ins) (ins->flags & 0x01 ? ins->loopend : ins->slength)
#define ins_getSampleRate(ins) (ins->c2speed)
#define ins_getSampleData(ins) (void *)MK_FP(ins->memseg, 0)

#define isSampleDataInEM(seg) (seg >= 0xf000)
#define getSampleDataLogPageInEM(seg) (seg & 0x0fff)

/*** Musical instruments list ***/

#define MAX_INSTRUMENTS 99
    /* 1..99 samples */

typedef MUSINS instrumentsList_t[MAX_INSTRUMENTS];

//#define insList_get(num) (mod_Instruments[(num)-1])
//#define insList_get(num) (MUSINS *)(MK_FP(FP_SEG(*mod_Instruments), FP_OFF(*mod_Instruments) + ((num) - 1) * sizeof(MUSINS)))
#define insList_get(num) (MUSINS *)MK_FP(FP_SEG(mod_Instruments[0]) + ((num) - 1) * 5, 0)

/*** Variables ***/

extern instrumentsList_t *PUBLIC_DATA mod_Instruments;  /* pointer to data for all instruments */
extern uint16_t PUBLIC_DATA InsNum;
extern bool     PUBLIC_DATA EMSSmp;         /* samples in EMS ? */
extern uint16_t PUBLIC_DATA SmpEMSHandle;   /* hanlde to access EMS for samples */

#endif  /* MUSINS_H */
