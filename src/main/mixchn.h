/* mixchn.h -- declarations for mixing channel handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MIXCHN_H
#define MIXCHN_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "cc/i86.h"
#include "main/s3mtypes.h"

/* Mixing channel */

#define MAX_CHANNELS 32
    /* 0..31 channels */

#define EFFFLAG_CONTINUE 0x01

typedef struct channel_t
{
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

#define chn_getState(o)            (o->bEnabled)
#define chn_setState(o, v)         o->bEnabled = v
#define _chn_setSamplePeriod(o, v) o->wSmpPeriod = v
#define chn_getSamplePeriod(o)     (o->wSmpPeriod)
#define chn_getSampleStep(o)       (o->dSmpStep)
#define chn_setSampleStep(o, v)    o->dSmpStep = v
#define chn_getInstrument(o)       (struct instrument_t *)MK_FP(o->wInsSeg, 0)
#define chn_setInstrument(o, p)    o->wInsSeg = FP_SEG((void __far *)p)
#define chn_setSampleData(o, p)    o->wSmpSeg = FP_SEG((void __far *)p)
#define chn_setCommand(o, v)       o->bCommand = v
#define chn_getCommand(o)          (o->bCommand)
#define chn_setSubCommand(o, v)    o->bCommand2 = v
#define chn_getSubCommand(o)       (o->bCommand2)
#define chn_setEffectParam(o, v)   o->bParameter = v
#define chn_getEffectParam(o)      (o->bParameter)

void     PUBLIC_CODE chn_setSampleVolume(MIXCHN *chn, int16_t vol);
uint16_t PUBLIC_CODE chn_checkSamplePeriod(MIXCHN *chn, uint32_t period);
void     PUBLIC_CODE chn_setSamplePeriod(MIXCHN *chn, uint32_t period);
void     PUBLIC_CODE chn_setPeriodLimits(MIXCHN *chn, uint16_t rate, bool amiga);

/* Mixing channels list */

typedef MIXCHN channelsList_t[MAX_CHANNELS];

/* Variables */

extern channelsList_t PUBLIC_DATA Channel;  /* all public/private data for every channel */
extern uint8_t PUBLIC_DATA UsedChannels;    /* possible values : 1..32 (kill all Adlib) */

#endif 	/* MIXCHN_H */
