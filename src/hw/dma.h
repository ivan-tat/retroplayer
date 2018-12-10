/* dma.h -- declarations for dma.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef DMA_H
#define DMA_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "hw/hwowner.h"

// TODO: remove PUBLIC_CODE macros when done.

#define DMA_CHANNELS 8

/*** Hardware I/O ***/

/* Transfer mode register */

typedef uint8_t dma_mode_t;
typedef dma_mode_t DMAMODE;

/* channel select */
#define DMA_MODE_CHAN_MASK    0x03
/* transfer type select */
#define DMA_MODE_TRAN_MASK    0x0c
#define DMA_MODE_TRAN_VERIFY  0x00
#define DMA_MODE_TRAN_WRITE   0x04
#define DMA_MODE_TRAN_READ    0x08
#define DMA_MODE_TRAN_ILLEGAL 0x0c
/* autoinitialization select */
#define DMA_MODE_INIT_MASK    0x10
#define DMA_MODE_INIT_AUTO    0x10
#define DMA_MODE_INIT_SINGLE  0x00
/* address increment/decrement select */
#define DMA_MODE_ADDR_MASK    0x20
#define DMA_MODE_ADDR_INCR    0x00
#define DMA_MODE_ADDR_DECR    0x20
/* mode select */
#define DMA_MODE_MASK         0xc0
#define DMA_MODE_DEMAND       0x00
#define DMA_MODE_SINGLE       0x40
#define DMA_MODE_BLOCK        0x80
#define DMA_MODE_CASCADE      0xc0

typedef uint8_t dma_mask_t;
typedef dma_mask_t DMAMASK;

uint32_t dma_get_linear_address(void *p);

/*** Sharing DMA channels ***/

DMAMASK   dma_get_hooked_channels(void);
HWOWNERID dma_get_owner(uint8_t ch);

bool     hwowner_hook_dma(HWOWNER *self, uint8_t ch);
bool     hwowner_hook_dma_channels(HWOWNER *self, DMAMASK mask);
bool     hwowner_mask_dma(HWOWNER *self, uint8_t ch);
bool     hwowner_mask_dma_channels(HWOWNER *self, DMAMASK mask);
bool     hwowner_enable_dma(HWOWNER *self, uint8_t ch);
bool     hwowner_enable_dma_channels(HWOWNER *self, DMAMASK mask);
bool     hwowner_setup_dma_transfer(HWOWNER *self, uint8_t ch, DMAMODE mode, uint32_t l, uint16_t count);
uint16_t hwowner_get_dma_counter(HWOWNER *self, uint8_t ch);
bool     hwowner_release_dma(HWOWNER *self, uint8_t ch);
bool     hwowner_release_dma_channels(HWOWNER *self, DMAMASK mask);

/*** Buffer ***/

typedef struct dma_buffer_t
{
    void    *data;
    uint32_t size;
    void    *unaligned;
};
typedef struct dma_buffer_t DMABUF;

void    PUBLIC_CODE dmaBuf_init(DMABUF *self);
bool    PUBLIC_CODE dmaBuf_alloc(DMABUF *self, uint32_t size);
void    PUBLIC_CODE dmaBuf_free(DMABUF *self);

/*** Initialization ***/

DECLARE_REGISTRATION (dma)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux dma_get_linear_address "*";

#pragma aux dma_get_hooked_channels "*";
#pragma aux dma_get_owner "*";

#pragma aux hwowner_hook_dma "*";
#pragma aux hwowner_hook_dma_channels "*";
#pragma aux hwowner_mask_dma "*";
#pragma aux hwowner_mask_dma_channels "*";
#pragma aux hwowner_enable_dma "*";
#pragma aux hwowner_enable_dma_channels "*";
#pragma aux hwowner_setup_dma_transfer "*";
#pragma aux hwowner_get_dma_counter "*";
#pragma aux hwowner_release_dma "*";
#pragma aux hwowner_release_dma_channels "*";

#pragma aux dmaBuf_init "*";
#pragma aux dmaBuf_alloc "*";
#pragma aux dmaBuf_free "*";

#pragma aux register_dma "*";
#pragma aux unregister_dma "*";

#endif  /* __WATCOMC__ */

#endif  /* DMA_H */
