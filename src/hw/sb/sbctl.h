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

typedef void SBDEV;

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

SBDEV   *sb_new(void);
void     sb_init(SBDEV *self);
void     sb_conf_manual(SBDEV *self, SBCFGFLAGS flags, SBMODEL model, uint16_t base, uint8_t irq, uint8_t dma8, uint8_t dma16);
bool     sb_conf_detect(SBDEV *self);
bool     sb_conf_env(SBDEV *self);
bool     sb_conf_input(SBDEV *self);
void     sb_conf_dump(SBDEV *self);
char    *sb_get_name(SBDEV *self);
void     sb_set_volume(SBDEV *self, uint8_t value);
bool     sb_adjust_transfer_mode(SBDEV *self, uint16_t *m_rate, uint8_t *m_channels, uint8_t *m_bits, bool *f_sign);
bool     sb_set_transfer_mode(SBDEV *self, uint16_t m_rate, uint8_t m_channels, uint8_t m_bits, bool f_sign);
bool     sb_set_transfer_buffer(SBDEV *self, void *buffer, uint16_t frame_size, uint16_t frames_count, bool autoinit, void *callback, void *cb_param);
uint8_t  sb_mode_get_bits(SBDEV *self);
bool     sb_mode_is_signed(SBDEV *self);
uint8_t  sb_mode_get_channels(SBDEV *self);
uint16_t sb_mode_get_rate(SBDEV *self);
bool     sb_transfer_start(SBDEV *self);
uint32_t sb_get_buffer_pos(SBDEV *self);
bool     sb_transfer_pause(SBDEV *self);
bool     sb_transfer_continue(SBDEV *self);
bool     sb_transfer_stop(SBDEV *self);
void     sb_free(SBDEV *self);
void     sb_delete(SBDEV **self);

/*** Initialization ***/

DECLARE_REGISTRATION (sbctl)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux sb_new "*";
#pragma aux sb_init "*";
#pragma aux sb_conf_manual "*";
#pragma aux sb_conf_detect "*";
#pragma aux sb_conf_env "*";
#pragma aux sb_conf_input "*";
#pragma aux sb_conf_dump "*";
#pragma aux sb_get_name "*";
#pragma aux sb_set_volume "*";
#pragma aux sb_set_transfer_buffer "*";
#pragma aux sb_adjust_transfer_mode "*";
#pragma aux sb_set_transfer_mode "*";
#pragma aux sb_mode_get_bits "*";
#pragma aux sb_mode_is_signed "*";
#pragma aux sb_mode_get_channels "*";
#pragma aux sb_mode_get_rate "*";
#pragma aux sb_transfer_start "*";
#pragma aux sb_get_buffer_pos "*";
#pragma aux sb_transfer_pause "*";
#pragma aux sb_transfer_continue "*";
#pragma aux sb_transfer_stop "*";
#pragma aux sb_free "*";
#pragma aux sb_delete "*";

#pragma aux register_sbctl "*";
#pragma aux unregister_sbctl "*";

#endif  /* __WATCOMC__ */

#endif  /* SBCTL_H */
