/* musdefs.h -- musical definitions.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _MUSDEFS_H_INCLUDED
#define _MUSDEFS_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "cc/i86.h"

/* Constants */

#define MID_C_PERIOD 1712
#define MID_C_RATE 8363

/* Instrument */

#define CHN_INS_NONE    0
#define CHN_INS_MIN     1
#define CHN_INS_MAX     99

#define _get_instrument(v)  ((v) - CHN_INS_MIN)

/* Note */

#define _mk_note(tone, octave)  ((octave) * 12 + (tone))

#define NOTE_MAX            _mk_note(11, 7)
#define NOTE_LINEAR_MAX     NOTE_MAX
#define NOTE_PACKED_MAX     _pack_note (NOTE_MAX)

#define CHN_NOTE_NONE   0xff
#define CHN_NOTE_OFF    0xfe
#define CHN_NOTE_MIN    0
#define CHN_NOTE_MAX    NOTE_PACKED_MAX

#define _pack_note(v)   (((v) % 12) + (((v) / 12) << 4))
#define _unpack_note(v) (((v) & 0x0f) + ((v) >> 4) * 12)

/* Note volume */

#define CHN_NOTEVOL_NONE    0xff
#define CHN_NOTEVOL_MAX     64

/* Command */

#define CHN_CMD_NONE    0xff
#define CHN_CMD_MIN     0
#define CHN_CMD_MAX     26

/*****************************************************************************/

/* song arrangement */

#define MAX_ORDERS 255
    /* 0..254 orders */

typedef uint8_t order_t;

typedef order_t ordersList_t[MAX_ORDERS];

#endif  /* !_MUSDEFS_H_INCLUDED */
