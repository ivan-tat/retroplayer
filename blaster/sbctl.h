/* sbctl.h -- declarations for sbctl.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _SBCTL_H
#define _SBCTL_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

/* hardware config */

extern char    *__pascal sdev_name[];       /* name */
extern uint16_t __pascal sdev_hw_base;      /* base i/o address */
extern uint8_t  __pascal sdev_hw_irq;       /* IRQ */
extern uint8_t  __pascal sdev_hw_dma8;      /* DMA channel for 8bit play */
extern uint8_t  __pascal sdev_hw_dma16;     /* DMA channel for 16bit play */

/* private */

extern uint8_t  __pascal sbno;              /* type */

/* capabilities */

extern bool     __pascal sdev_caps_mixer;
extern bool     __pascal sdev_caps_16bit;           /* 16bit play is possible */
extern bool     __pascal sdev_caps_stereo;          /* stereo play is possible */
extern uint16_t __pascal sdev_caps_mono_maxrate;    /* max mono samplerate */
extern uint16_t __pascal sdev_caps_stereo_maxrate;  /* max stereo samplerate */

/* transfer mode */

extern uint16_t __pascal sdev_mode_rate;
extern bool     __pascal _16bit;
extern bool     __pascal sdev_mode_signed;
extern bool     __pascal stereo;

void     __far __pascal setvolume( uint8_t vol );
uint16_t __far __pascal sbReadDSPVersion( void );
void     __far __pascal sbAdjustMode( uint16_t *rate, bool *stereo, bool *_16bit );
void     __far __pascal sbSetupMode( uint16_t freq, bool stereo );
void     __far __pascal sbSetupDMATransfer( void *p, uint16_t count, bool autoinit );
uint16_t __far __pascal sbGetDMACounter( void );
void     __far __pascal sbSetupDSPTransfer( uint16_t len, bool autoinit );
void     __far __pascal pause_play( void );
void     __far __pascal continue_play( void );
void     __far __pascal stop_play( void );

#endif /* _SBCTL_H */
