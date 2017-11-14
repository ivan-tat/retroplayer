/* sbctl.h -- declarations for sbctl.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef SBCTL_H
#define SBCTL_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "hw/sb/sndisr_.h"

// TODO: remove PUBLIC_DATA and PUBLIC_CODE macros when done.

/* transfer mode */

extern uint16_t PUBLIC_DATA sdev_mode_rate;
extern bool     PUBLIC_DATA sdev_mode_16bits;
extern bool     PUBLIC_DATA sdev_mode_signed;
extern bool     PUBLIC_DATA sdev_mode_stereo;

void     PUBLIC_CODE sb_set_volume(uint8_t value);
void     PUBLIC_CODE sbAdjustMode(uint16_t *rate, bool *stereo, bool *_16bits);
void     PUBLIC_CODE sbSetupMode(uint16_t freq, bool stereo);
void     PUBLIC_CODE sbSetupDMATransfer(void *p, uint16_t count, bool autoinit);
uint16_t PUBLIC_CODE sbGetDMACounter(void);
void     PUBLIC_CODE sbSetupDSPTransfer(uint16_t len, bool autoinit);
void     PUBLIC_CODE Initblaster(bool *f_16bits, bool *f_stereo, uint16_t *rate);
void     PUBLIC_CODE pause_play(void);
void     PUBLIC_CODE continue_play(void);
void     PUBLIC_CODE stop_play(void);

void     PUBLIC_CODE Forceto(uint8_t type, uint8_t dma8, uint8_t dma16, uint8_t irq, uint16_t dsp);
bool     PUBLIC_CODE Detect_DSP_Addr(void);
bool     PUBLIC_CODE Detect_DMA_Channel_IRQ(void);
bool     PUBLIC_CODE DetectSoundblaster(void);
bool     PUBLIC_CODE InputSoundblasterValues(void);
bool     PUBLIC_CODE UseBlasterEnv(void);
void     PUBLIC_CODE set_ready_irq(void *p);
void     PUBLIC_CODE restore_irq(void);
void     PUBLIC_CODE writelnSBConfig(void);

/* Initialization */

DECLARE_REGISTRATION(sbctl)

#endif  /* SBCTL_H */
