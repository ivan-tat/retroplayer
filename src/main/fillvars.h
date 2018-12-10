/* fillvars.h -- declarations for fillvars.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef FILLVARS_H
#define FILLVARS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "hw/dma.h"
#include "hw/sndctl_t.h"

// TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done.

/* DMA buffer for sound */

typedef uint16_t sound_DMA_buffer_flags_t;
typedef sound_DMA_buffer_flags_t SNDDMABUFFLAGS;

#define SNDDMABUFFL_LOCKED (1<<0)
#define SNDDMABUFFL_SLOW   (1<<1)
#define SNDDMABUFFL_LQ     (1<<2)

#define DMA_BUF_SIZE_MAX (8<<10)

#pragma pack(push, 1);
typedef struct sound_DMA_buffer_t
{
    SNDDMABUFFLAGS flags;
    DMABUF  *buf;
    HWSMPFMT format;
    uint16_t frameSize;
    uint8_t  framesCount;
    uint8_t  frameLast;
    uint8_t  frameActive;
};
#pragma pack(pop);
typedef struct sound_DMA_buffer_t SNDDMABUF;

/* player */

extern uint8_t playOption_FPS;
    /* frames per second ... default is about 70Hz */

void     PUBLIC_CODE snddmabuf_init(SNDDMABUF *self);
bool     PUBLIC_CODE snddmabuf_alloc(SNDDMABUF *self, uint32_t dmaSize);
uint16_t PUBLIC_CODE snddmabuf_get_frame_offset(SNDDMABUF *self, uint8_t index);
void    *PUBLIC_CODE snddmabuf_get_frame(SNDDMABUF *self, uint8_t index);
uint16_t PUBLIC_CODE snddmabuf_get_offset_from_count(SNDDMABUF *self, uint16_t count);
uint16_t PUBLIC_CODE snddmabuf_get_count_from_offset(SNDDMABUF *self, uint16_t bufOff);
void     PUBLIC_CODE snddmabuf_free(SNDDMABUF *self);

extern SNDDMABUF sndDMABuf;

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux playOption_FPS "*";

#pragma aux snddmabuf_init "*";
#pragma aux snddmabuf_alloc "*";
#pragma aux snddmabuf_get_frame_offset "*";
#pragma aux snddmabuf_get_frame "*";
#pragma aux snddmabuf_get_offset_from_count "*";
#pragma aux snddmabuf_get_count_from_offset "*";
#pragma aux snddmabuf_free "*";

#pragma aux sndDMABuf "*";

#endif  /* __WATCOMC__ */

#endif  /* FILLVARS_H */
