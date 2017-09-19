/* pic.h -- declarations for pic.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef PIC_H
#define PIC_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_CODE macros when done.

#include "pascal/pascal.h"

/* Hardware I/O */

#define IRQ_CHANNELS 16

typedef uint16_t irqMask_t;

void  PUBLIC_CODE picEnableChannels(irqMask_t mask);
void  PUBLIC_CODE picDisableChannels(irqMask_t mask);
void  PUBLIC_CODE picEOI(uint8_t ch);
void *PUBLIC_CODE picGetISR(uint8_t ch);
void  PUBLIC_CODE picSetISR(uint8_t ch, void *p);

/* Sharing IRQ channels */

#include "isr_t.h"

extern void PUBLIC_CODE isrHookSingleChannel(uint8_t ch);
extern void PUBLIC_CODE isrReleaseSingleChannel(uint8_t ch);
extern void PUBLIC_CODE isrSetSingleChannelHandler(uint8_t ch, isrCallback_t *p);
extern isrCallback_t *PUBLIC_CODE isrGetSingleChannelHandler(uint8_t ch);
extern void PUBLIC_CODE isrClearSingleChannelHandler(uint8_t ch);

/* Initialization */

DECLARE_REGISTRATION(pic)

#endif  /* PIC_H */
