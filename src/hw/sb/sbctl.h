/* sbctl.h -- declarations for sbctl.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef SBCTL_H
#define SBCTL_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

// TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done.

#include "pascal/pascal.h"

/* hardware config */

extern char    *PUBLIC_DATA sdev_name[];       /* name */
extern uint16_t PUBLIC_DATA sdev_hw_base;      /* base i/o address */
extern uint8_t  PUBLIC_DATA sdev_hw_irq;       /* IRQ */
extern uint8_t  PUBLIC_DATA sdev_hw_dma8;      /* DMA channel for 8bit play */
extern uint8_t  PUBLIC_DATA sdev_hw_dma16;     /* DMA channel for 16bit play */

/* private */

extern uint8_t  PUBLIC_DATA sbno;              /* type */

/* capabilities */

extern bool     PUBLIC_DATA sdev_caps_mixer;
extern bool     PUBLIC_DATA sdev_caps_16bit;           /* 16bit play is possible */
extern bool     PUBLIC_DATA sdev_caps_stereo;          /* stereo play is possible */
extern uint16_t PUBLIC_DATA sdev_caps_mono_maxrate;    /* max mono samplerate */
extern uint16_t PUBLIC_DATA sdev_caps_stereo_maxrate;  /* max stereo samplerate */

/* transfer mode */

extern uint16_t PUBLIC_DATA sdev_mode_rate;
extern bool     PUBLIC_DATA sdev_mode_16bit;
extern bool     PUBLIC_DATA sdev_mode_signed;
extern bool     PUBLIC_DATA sdev_mode_stereo;

void     PUBLIC_CODE setvolume(uint8_t vol);
uint16_t PUBLIC_CODE sbReadDSPVersion(void);
void     PUBLIC_CODE sbAdjustMode(uint16_t *rate, bool *stereo, bool *_16bit);
void     PUBLIC_CODE sbSetupMode(uint16_t freq, bool stereo);
void     PUBLIC_CODE sbSetupDMATransfer(void *p, uint16_t count, bool autoinit);
uint16_t PUBLIC_CODE sbGetDMACounter(void);
void     PUBLIC_CODE sbSetupDSPTransfer(uint16_t len, bool autoinit);
void     PUBLIC_CODE pause_play(void);
void     PUBLIC_CODE continue_play(void);
void     PUBLIC_CODE stop_play(void);

#endif  /* SBCTL_H */
