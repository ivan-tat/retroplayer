/* fillvars.h -- declarations for fillvars.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef FILLVARS_H
#define FILLVARS_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

/* DMA buffer */

#define DMA_BUF_SIZE_MAX (8<<10)

extern void    *PUBLIC_DATA DMABufUnaligned;
extern void    *PUBLIC_DATA DMABuf;
extern uint32_t PUBLIC_DATA DMABufSize;
extern uint16_t PUBLIC_DATA DMABufFrameSize;
extern uint8_t  PUBLIC_DATA DMABufFramesCount;
extern uint8_t  PUBLIC_DATA DMABufFrameLast;
extern uint8_t  PUBLIC_DATA DMABufFrameActive;
extern bool     PUBLIC_DATA DMAFlags_JustInFill;
extern bool     PUBLIC_DATA DMAFlags_Slow;

/* player */

extern uint8_t PUBLIC_DATA playOption_FPS;
    /* frames per second ... default is about 70Hz */
extern bool    PUBLIC_DATA playOption_LowQuality;
    /* flag if lowquality mode */

/* EMM */

extern uint16_t PUBLIC_DATA SavHandle;
    /* EMS handle for saving mapping while playing */

extern uint16_t PUBLIC_CODE getDMABufFrameOff(uint8_t index);
extern uint16_t PUBLIC_CODE getDMABufOffFromCount(uint16_t count);
extern uint16_t PUBLIC_CODE getCountFromDMABufOff(uint16_t bufOff);
extern void     PUBLIC_CODE initDMABuf(void);
extern bool     PUBLIC_CODE allocDMABuf(uint32_t dmaSize);
extern void     PUBLIC_CODE freeDMABuf(void);
extern void     PUBLIC_CODE doneDMABuf(void);

#endif  /* FILLVARS_H */
