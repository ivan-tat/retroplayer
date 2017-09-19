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

#include "../pascal/pascal.h"
#include "../hw/dma.h"
#include "../hw/sndctl_t.h"

/* DMA buffer for sound */

#define DMA_BUF_SIZE_MAX (8<<10)

typedef struct soundDMABuffer_t {
    DMABUF  *buf;
    HWSMPFMT format;
    uint16_t frameSize;
    uint8_t  framesCount;
    uint8_t  frameLast;
    uint8_t  frameActive;
    bool     flags_locked;
    bool     flags_Slow;
};
typedef struct soundDMABuffer_t SNDDMABUF;

extern SNDDMABUF PUBLIC_DATA sndDMABuf;

/* player */

extern uint8_t PUBLIC_DATA playOption_FPS;
    /* frames per second ... default is about 70Hz */
extern bool    PUBLIC_DATA playOption_LowQuality;

/* EMM */

extern uint16_t PUBLIC_DATA SavHandle;
    /* EMS handle for saving mapping while playing */

//SNDDMABUF *PUBLIC_CODE sndDMABuf_new(void);
//SNDDMABUF *PUBLIC_CODE sndDMABuf_copy(SNDDMABUF *instance);
//void     PUBLIC_CODE sndDMABuf_delete(SNDDMABUF **instance);

uint16_t PUBLIC_CODE sndDMABufGetFrameOff(SNDDMABUF *buf, uint8_t index);
uint16_t PUBLIC_CODE sndDMABufGetOffFromCount(SNDDMABUF *buf, uint16_t count);
uint16_t PUBLIC_CODE sndDMABufGetCountFromOff(SNDDMABUF *buf, uint16_t bufOff);
bool     PUBLIC_CODE sndDMABufAlloc(SNDDMABUF *buf, uint32_t dmaSize);
void     PUBLIC_CODE sndDMABufFree(SNDDMABUF *buf);

void     PUBLIC_CODE sndDMABufInit(SNDDMABUF *buf);
void     PUBLIC_CODE sndDMABufDone(SNDDMABUF *buf);

#endif  /* FILLVARS_H */
