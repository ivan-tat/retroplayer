/* dma.h -- declarations for dma.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef DMA_H
#define DMA_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "..\pascal\pascal.h"

/* DMA transfer mode register */

typedef uint8_t DMAMode_t;

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

void     PUBLIC_CODE dmaMaskMulti(uint8_t mask);
void     PUBLIC_CODE dmaMask(uint8_t ch);
void     PUBLIC_CODE dmaEnableMulti(uint8_t mask);
void     PUBLIC_CODE dmaEnable(uint8_t ch);
uint32_t PUBLIC_CODE dmaGetLinearAddress(void *p);
void     PUBLIC_CODE dmaSetup(uint8_t ch, DMAMode_t mode, void *p, uint16_t count);
uint16_t PUBLIC_CODE dmaGetCounter(uint8_t ch);

#endif /* DMA_H */
