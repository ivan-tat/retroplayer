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

/* instrument */

#define MAX_INSTRUMENTS 99
    /* 1..99 samples */

#define SMPFLAG_LOOP 0x01

typedef struct instrument_t {
    uint8_t  bType;         // 1: instrument
    uint8_t  DOSname[12];
    uint8_t  unused1;
    uint16_t memseg;        // segment of sampledata
    uint16_t slength;       // length of sample <= 64KB
    uint16_t HI_length;     // <- not used !
    uint16_t loopbeg;       // loop begin <= 64KB
    uint16_t HI_loopbeg;    // <- not used !
    uint16_t loopend;       // loop end <= 64KB
    uint16_t HI_loopend;    // <- not used !
    uint8_t  vol;           // default instrument volume
    uint8_t  unused2;
    uint8_t  packinfo;      // =0 or we can't play it !
    uint8_t  flags;         // bit 0: loop (all other bits are ignored)
    uint16_t c2speed;       // frequency at middle C (actually C-4)
    uint16_t c2speed_hi;    // ignored
    uint8_t  unused3[12];
    uint8_t  IName[28];
    uint32_t SCRS_ID;
};

typedef struct instrument_t instrumentsList_t[MAX_INSTRUMENTS];

/* song arrangement */

#define MAX_ORDERS 255
    /* 0..254 orders */

typedef uint8_t order_t;

typedef order_t ordersList_t[MAX_ORDERS];

#endif  /* S3MTYPES_H */
