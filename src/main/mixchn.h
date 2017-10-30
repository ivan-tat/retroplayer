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
#include "main/musins.h"
#include "main/s3mtypes.h"

/* Mixing channel */

typedef uint8_t MIXCHNFLAGS;

#define MIXCHNFL_ENABLED (1<<0) // channel is enabled (can play voice, do effects), otherwise ignored
#define MIXCHNFL_PLAYING (1<<1) // playing sample, otherwise passive
#define MIXCHNFL_MIXING  (1<<2) // mix output, otherwise muted

#define EFFFLAG_CONTINUE 0x01

typedef struct mix_channel_t
{
    MIXCHNFLAGS bChannelFlags;
    uint8_t  bChannelType;  // 0=off, 1=left, 2=right, 3,4=adlib (if 0,3,4 -> everything ignored !)
    // current Instrument :
    MUSINS  *pMusIns;
    uint16_t wSmpSeg;       // DOS segment of current sample data
    uint8_t  bIns;          // number of instrument is currently playing
    uint8_t  bNote;
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

typedef struct mix_channel_t MIXCHN;

void     PUBLIC_CODE mixchn_set_flags(MIXCHN *self, MIXCHNFLAGS value);
MIXCHNFLAGS PUBLIC_CODE mixchn_get_flags(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_enabled(MIXCHN *self, bool value);
bool     PUBLIC_CODE mixchn_is_enabled(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_playing(MIXCHN *self, bool value);
bool     PUBLIC_CODE mixchn_is_playing(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_mixing(MIXCHN *self, bool value);
bool     PUBLIC_CODE mixchn_is_mixing(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_type(MIXCHN *self, uint8_t value);
uint8_t  PUBLIC_CODE mixchn_get_type(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_instrument_num(MIXCHN *self, uint8_t value);
uint8_t  PUBLIC_CODE mixchn_get_instrument_num(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_instrument(MIXCHN *self, MUSINS *value);
MUSINS  *PUBLIC_CODE mixchn_get_instrument(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_sample_volume(MIXCHN *self, int16_t vol);
uint8_t  PUBLIC_CODE mixchn_get_sample_volume(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_sample_period_limits(MIXCHN *self, uint16_t rate, bool amiga);
uint16_t PUBLIC_CODE mixchn_check_sample_period(MIXCHN *self, uint32_t value);
void     PUBLIC_CODE mixchn_set_sample_period(MIXCHN *self, uint16_t value);
uint16_t PUBLIC_CODE mixchn_get_sample_period(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_sample_step(MIXCHN *self, uint32_t value);
uint32_t PUBLIC_CODE mixchn_get_sample_step(MIXCHN *self);
void     PUBLIC_CODE mixchn_setup_sample_period(MIXCHN *self, uint32_t value);
void     PUBLIC_CODE mixchn_set_sample_data(MIXCHN *self, void *value);
void    *PUBLIC_CODE mixchn_get_sample_data(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_command(MIXCHN *self, uint8_t value);
uint8_t  PUBLIC_CODE mixchn_get_command(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_sub_command(MIXCHN *self, uint8_t value);
uint8_t  PUBLIC_CODE mixchn_get_sub_command(MIXCHN *self);
void     PUBLIC_CODE mixchn_set_command_parameter(MIXCHN *self, uint8_t value);
uint8_t  PUBLIC_CODE mixchn_get_command_parameter(MIXCHN *self);
void     PUBLIC_CODE chn_setupInstrument(MIXCHN *chn, uint8_t insNum);
uint16_t PUBLIC_CODE chn_calcNotePeriod(MIXCHN *chn, uint32_t rate, uint8_t note);
uint32_t PUBLIC_CODE chn_calcNoteStep(MIXCHN *chn, uint32_t rate, uint8_t note);
void     PUBLIC_CODE chn_setupNote(MIXCHN *chn, uint8_t note, bool keep);


/* Mixing channels list */

#define MAX_CHANNELS 32
    /* 0..31 channels */

typedef MIXCHN channelsList_t[MAX_CHANNELS];

/* Variables */

extern channelsList_t PUBLIC_DATA Channel;  /* all public/private data for every channel */
extern uint8_t PUBLIC_DATA UsedChannels;    /* possible values : 1..32 (kill all Adlib) */

#endif 	/* MIXCHN_H */
