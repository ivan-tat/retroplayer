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

/* hardware config */

extern uint8_t  PUBLIC_DATA sdev_type;         /* type (private) */
extern char    *PUBLIC_DATA sdev_name;         /* name */
extern uint16_t PUBLIC_DATA sdev_hw_base;      /* base i/o address */
extern uint8_t  PUBLIC_DATA sdev_hw_irq;       /* IRQ */
extern uint8_t  PUBLIC_DATA sdev_hw_dma8;      /* DMA channel for 8bit play */
extern uint8_t  PUBLIC_DATA sdev_hw_dma16;     /* DMA channel for 16bit play */
extern bool     PUBLIC_DATA sdev_configured;   /* sound card is detected */
extern bool     PUBLIC_DATA sdev_hwflags_base; /* DSP base I/O address is detected */
extern bool     PUBLIC_DATA sdev_hwflags_irq;  /* IRQ channel is detected */
extern bool     PUBLIC_DATA sdev_hwflags_dma8; /* DMA 8-bits channel is detected */
extern bool     PUBLIC_DATA sdev_hwflags_dma16;/* DMA 16-bits channel is detected */
extern uint16_t PUBLIC_DATA sdev_hw_dspv;      /* DSP chip version */
extern uint8_t  PUBLIC_DATA sdev_irq_answer;   /* for detecting (private) */
extern void    *PUBLIC_DATA sdev_irq_savedvec; /* for detecting (private) */

extern SoundHWISRCallback_t *PUBLIC_DATA ISRUserCallback;   /* private */

/* capabilities */

extern bool     PUBLIC_DATA sdev_caps_mixer;
extern bool     PUBLIC_DATA sdev_caps_16bit;           /* 16bit play is possible */
extern bool     PUBLIC_DATA sdev_caps_stereo;          /* stereo play is possible */
extern uint16_t PUBLIC_DATA sdev_caps_mono_maxrate;    /* max mono samplerate */
extern uint16_t PUBLIC_DATA sdev_caps_stereo_maxrate;  /* max stereo samplerate */

/* transfer mode */

extern uint16_t PUBLIC_DATA sdev_mode_rate;
extern bool     PUBLIC_DATA sdev_mode_16bits;
extern bool     PUBLIC_DATA sdev_mode_signed;
extern bool     PUBLIC_DATA sdev_mode_stereo;

void     PUBLIC_CODE setvolume(uint8_t vol);
uint16_t PUBLIC_CODE sbReadDSPVersion(void);
void     PUBLIC_CODE sbAdjustMode(uint16_t *rate, bool *stereo, bool *_16bit);
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
void     PUBLIC_CODE set_ready_irq(void *p);
void     PUBLIC_CODE restore_irq(void);
void     PUBLIC_CODE writelnSBConfig(void);

/* Initialization */

DECLARE_REGISTRATION(sbctl)

#endif  /* SBCTL_H */
