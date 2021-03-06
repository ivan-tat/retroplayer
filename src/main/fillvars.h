/* fillvars.h -- declarations for fillvars.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _FILLVARS_H_INCLUDED
#define _FILLVARS_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "hw/dma.h"
#include "hw/sndctl_t.h"

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
    int8_t   frameLast;
    uint8_t  frameActive;
};
#pragma pack(pop);
typedef struct sound_DMA_buffer_t SNDDMABUF;

/* player */

void     __far snddmabuf_init (SNDDMABUF *self);
bool     __far snddmabuf_alloc (SNDDMABUF *self, uint32_t dmaSize);
uint16_t __far snddmabuf_get_frame_offset (SNDDMABUF *self, uint8_t index);
void    *__far snddmabuf_get_frame (SNDDMABUF *self, uint8_t index);
uint16_t __far snddmabuf_get_offset_from_count (SNDDMABUF *self, uint16_t count);
uint16_t __far snddmabuf_get_count_from_offset (SNDDMABUF *self, uint16_t bufOff);
void     __far snddmabuf_free (SNDDMABUF *self);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux snddmabuf_init "*";
#pragma aux snddmabuf_alloc "*";
#pragma aux snddmabuf_get_frame_offset "*";
#pragma aux snddmabuf_get_frame "*";
#pragma aux snddmabuf_get_offset_from_count "*";
#pragma aux snddmabuf_get_count_from_offset "*";
#pragma aux snddmabuf_free "*";

#endif  /* __WATCOMC__ */

#endif  /* !_FILLVARS_H_INCLUDED */
