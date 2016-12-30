/* dma.h -- declarations for dma.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef DMA_H
#define DMA_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

#define DMA_CHANNELS 8

/*** Hardware I/O ***/

/* Transfer mode register */

typedef uint8_t dmaMode_t;

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

typedef uint8_t dmaMask_t;

void     PUBLIC_CODE dmaMaskSingleChannel(uint8_t ch);
void     PUBLIC_CODE dmaMaskChannels(dmaMask_t mask);
void     PUBLIC_CODE dmaEnableSingleChannel(uint8_t ch);
void     PUBLIC_CODE dmaEnableChannels(dmaMask_t mask);
uint32_t PUBLIC_CODE dmaGetLinearAddress(void *p);
void     PUBLIC_CODE dmaSetupSingleChannel(uint8_t ch, dmaMode_t mode, uint32_t l, uint16_t count);
uint16_t PUBLIC_CODE dmaGetCounter(uint8_t ch);

/*** Sharing DMA channels ***/

typedef void dmaOwner_t;

bool        PUBLIC_CODE dmaIsAvailableSingleChannel(uint8_t ch);
dmaMask_t   PUBLIC_CODE dmaGetAvailableChannels(void);
dmaOwner_t *PUBLIC_CODE dmaGetSingleChannelOwner(uint8_t ch);
void        PUBLIC_CODE dmaHookSingleChannel(uint8_t ch, dmaOwner_t *owner);
void        PUBLIC_CODE dmaHookChannels(dmaMask_t mask, dmaOwner_t *owner);
void        PUBLIC_CODE dmaReleaseSingleChannel(uint8_t ch);
void        PUBLIC_CODE dmaReleaseChannels(dmaMask_t mask);

/*** Buffer ***/

typedef struct dmaBuffer_t {
    void    *data;
    uint32_t size;
    void    *unaligned;
};
typedef struct dmaBuffer_t DMABUF;

DMABUF *PUBLIC_CODE dmaBuf_new(void);
void    PUBLIC_CODE dmaBuf_delete(DMABUF **buf);
bool    PUBLIC_CODE dmaBufAlloc(DMABUF *buf, uint32_t size);
void    PUBLIC_CODE dmaBufFree(DMABUF *buf);

void    PUBLIC_CODE dmaBufInit(DMABUF *buf);
void    PUBLIC_CODE dmaBufDone(DMABUF *buf);

/*** Initialization ***/

DECLARE_REGISTRATION(dma)

#endif  /* DMA_H */
