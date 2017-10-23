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

#define NOTE_MAX (7 * 12 + 11)

#define CHNNOTE_MAX ((7 << 4) + 11)
#define CHNNOTE_EMPTY 0xff
#define CHNNOTE_OFF 0xfe

#define _isNote(note) (((note) != CHNNOTE_OFF) && ((note) != CHNNOTE_EMPTY))
#define _packNote(note) (((note) % 12) + (((note) / 12) << 4))
#define _unpackNote(note) (((note) & 0x0f) + ((note) >> 4) * 12)

#define CHNINSVOL_EMPTY 0xff
#define CHNINSVOL_MAX 63

#define _isVolume(vol) ((vol) != CHNINSVOL_EMPTY)

#define MID_C_RATE 8363

/* generic module information */

#define MOD_MAX_TITLE_LENGTH 29
    /* including trailing zero */

typedef char modTitle_t[MOD_MAX_TITLE_LENGTH];

#define MOD_MAX_TRACKER_NAME_LENGTH 32
    /* including trailing zero */

typedef char modTrackerName_t[MOD_MAX_TRACKER_NAME_LENGTH];

/* song arrangement */

#define MAX_ORDERS 255
    /* 0..254 orders */

typedef uint8_t order_t;

typedef order_t ordersList_t[MAX_ORDERS];

#endif  /* S3MTYPES_H */
