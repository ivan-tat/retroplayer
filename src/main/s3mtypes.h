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

/* channels */

#define MAX_CHANNELS 32
    /* 0..31 channels */

#define EFFFLAG_CONTINUE 0x01

typedef struct channel_t {
    // general switches :
    uint8_t  bEnabled;      // flag if =0 then nothing to mix at the moment
    uint8_t  bChannelType;  // 0=off, 1=left, 2=right, 3,4=adlib (if 0,3,4 -> everything ignored !)
    // current Instrument :
    uint16_t wInsSeg;       // DOS segment of current instrument data
    uint16_t wSmpSeg;       // DOS segment of current sample data
    uint8_t  bIns;          // number of instrument is currently playing
    uint8_t  bNote;         // we don't need it really for playing, but let's store it anyway
    // copy of sampledata (maybe it differs a bit):
    uint8_t  bSmpVol;       // current sample volume
    uint8_t  bSmpFlags;     // flags (looped sample)
    uint16_t wSmpStart;     // start offset of sample
    uint16_t wSmpLoopStart; // loop start of current sample (0xffff if no loop)
    uint16_t wSmpLoopEnd;   // normal or loop end of current sample
    uint32_t dSmpPos;       // fixed point 16:16 value for current position in sample
    uint32_t dSmpStep;      // fixed point 16:16 value of frequency step
                            // (distance of one step depends on period we play currently)
    uint16_t wSmpPeriod;    // st3 period ... you know these amiga values (look at tech.doc of ST3)
                            // ( period does no influence playing a sample direct, but it's for sliding etc.)
    uint16_t wSmpPeriodLow; // B-7 or B-5 period for current instrument to check limits
    uint16_t wSmpPeriodHigh;// C-0 or C-3 period for current instrument to check limits
    // effect info :
    uint8_t  bCommand;      // effect number (for using a jmptable)
    uint8_t  bCommand2;     // internal 2nd command part - for multiple effects
    uint8_t  bParameter;    // just the command parameters
    // extra effect data :
    uint8_t  bEffFlags;     // multiple effects: flags (continue for Vibrato and Tremolo)
    uint16_t wVibTab;       // Vibrato: offset of wavetable (default: sinuswave)
    uint16_t wTrmTab;       // Tremolo: offset of wavetable (default: sinuswave)
    uint8_t  bTabPos;       // multiple effects: we reset this if an effect starts uses such a table
    uint8_t  bVibParam;     // Vibrato   +VolSlide: parameter
    uint8_t  bPortParam;    // Portamento+VolSlide: parameter
    uint16_t wSmpPeriodOld; // multiple effects: save that value
    uint8_t  bSmpVolOld;    // Tremolo: save that value
    uint16_t wSmpPeriodDest;// Portamento: period to slide to
    uint8_t  bArpPos;       // Arpeggio: which of thoses 3 notes we currently play
    uint8_t  bArpNotes[2];  // Arpeggio: +note - 3 notes we do arpeggio between
    uint32_t dArpSmpSteps[3];// Arpeggio: 3 step values we switch between (0 is start value <- we have to refesh after Arpeggio)
    uint8_t  bRetrigTicks;  // Retrigger: ticks left to retrigg note
    uint8_t  bSavNote;      // NoteDelay: new value
    uint8_t  bSavIns;       // NoteDelay: new value
    uint8_t  bSavVol;       // NoteDelay: new value
    uint8_t  bDelayTicks;   // NoteDelay: new value | NoteCut: ticks left to cut
};

typedef struct channel_t MIXCHN;

typedef MIXCHN channelsList_t[MAX_CHANNELS];

#endif  /* S3MTYPES_H */
