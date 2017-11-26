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

typedef uint8_t SBMODEL;

#define SBMODEL_UNKNOWN 0
#define SBMODEL_SB1     1
#define SBMODEL_SB2     2
#define SBMODEL_SBPRO   3
#define SBMODEL_SB16    4

typedef uint8_t SBCFGFLAGS;

#define SBCFGFL_TYPE    (1 << 0)
#define SBCFGFL_BASE    (1 << 1)
#define SBCFGFL_IRQ     (1 << 2)
#define SBCFGFL_DMA8    (1 << 3)
#define SBCFGFL_DMA16   (1 << 4)
#define SBCFGFL_BASE_MASK (SBCFGFL_TYPE | SBCFGFL_BASE | SBCFGFL_IRQ | SBCFGFL_DMA8)

char    *PUBLIC_CODE sb_get_name(void);
uint8_t  PUBLIC_CODE sb_get_sample_bits(void);
bool     PUBLIC_CODE sb_is_sample_signed(void);
uint8_t  PUBLIC_CODE sb_get_channels(void);
uint16_t PUBLIC_CODE sb_get_rate(void);

void     PUBLIC_CODE sb_hook_IRQ(void *p);
void     PUBLIC_CODE sb_unhook_IRQ(void);
void     PUBLIC_CODE sb_set_volume(uint8_t value);
void     PUBLIC_CODE sb_set_transfer_buffer(void *buffer, uint16_t frame_size, uint16_t frames_count, bool autoinit);
void     PUBLIC_CODE sb_adjust_transfer_mode(uint16_t *m_rate, uint8_t *m_channels, uint8_t *m_bits, bool *f_sign);
void     PUBLIC_CODE sb_set_transfer_mode(uint16_t m_rate, uint8_t m_channels, uint8_t m_bits, bool f_sign);
bool     PUBLIC_CODE sb_transfer_start(void);
uint16_t PUBLIC_CODE sb_get_DMA_counter(void);
void     PUBLIC_CODE sb_transfer_pause(void);
void     PUBLIC_CODE sb_transfer_continue(void);
void     PUBLIC_CODE sb_transfer_stop(void);

void     PUBLIC_CODE sb_conf_manual(SBCFGFLAGS flags, SBMODEL model, uint16_t base, uint8_t irq, uint8_t dma8, uint8_t dma16);
bool     PUBLIC_CODE sb_conf_detect(void);
bool     PUBLIC_CODE sb_conf_env(void);
bool     PUBLIC_CODE sb_conf_input(void);
void     PUBLIC_CODE sb_conf_dump(void);

/* Initialization */

DECLARE_REGISTRATION(sbctl)

#endif  /* SBCTL_H */
