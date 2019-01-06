/* s3mtypes.h -- generic s3m types.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef S3MTYPES_H
#define S3MTYPES_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "cc/i86.h"

/* Constants */

#define MID_C_RATE  8363

/* Note */

#define CHN_NOTE_NONE   0xff
#define CHN_NOTE_OFF    0xfe
#define MAX_CHN_NOTE_LINEAR (7 * 12 + 11)
#define MAX_CHN_NOTE_PACKED ((7 << 4) + 11)

#define _is_note(v)     (((v) != CHN_NOTE_OFF) && ((v) != CHN_NOTE_NONE))
#define _pack_note(v)   (((v) % 12) + (((v) / 12) << 4))
#define _unpack_note(v) (((v) & 0x0f) + ((v) >> 4) * 12)

/* Instrument */

#define CHN_INS_NONE    0
#define MAX_CHN_INS     99

#define _is_instrument(v)   (((v) != CHN_INS_NONE) && ((v) <= MAX_CHN_INS))
#define _get_instrument(v)  ((v) - 1)

/* Volume */

#define CHN_VOL_NONE        0xff
#define MAX_CHN_VOL         64
#define MAX_CHN_VOL_HARD    63

#define _is_volume(v)   ((v) <= MAX_CHN_VOL)
#define _get_volume(v)  (v)

/*****************************************************************************/

#define NOTE_MAX    (7 * 12 + 11)

#define SMPVOL_MAX  64

/* song arrangement */

#define MAX_ORDERS 255
    /* 0..254 orders */

typedef uint8_t order_t;

typedef order_t ordersList_t[MAX_ORDERS];

#endif  /* S3MTYPES_H */
