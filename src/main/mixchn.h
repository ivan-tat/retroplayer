/* mixchn.h -- declarations for mixchn.c.

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
#include "dynarray.h"
#include "main/musins.h"
#include "main/s3mtypes.h"

/* Mixing channel */

typedef uint8_t mix_channel_type_t;
typedef mix_channel_type_t MIXCHNTYPE;

#define MIXCHNTYPE_NONE     0
#define MIXCHNTYPE_PCM      1
#define MIXCHNTYPE_ADLIB    2

typedef uint8_t MIXCHNFLAGS;

#define MIXCHNFL_ENABLED (1<<0) // channel is enabled (can play voice, do effects), otherwise ignored
#define MIXCHNFL_PLAYING (1<<1) // playing sample, otherwise passive
#define MIXCHNFL_MIXING  (1<<2) // mix output, otherwise muted

#define EFFFLAG_CONTINUE 0x01

typedef uint8_t mix_channel_pan_t;
typedef mix_channel_pan_t MIXCHNPAN;

#define MIXCHNPAN_LEFT      0
#define MIXCHNPAN_CENTER    32
#define MIXCHNPAN_RIGHT     64

typedef struct mix_channel_t
{
    MIXCHNTYPE type;
    MIXCHNFLAGS flags;
    MIXCHNPAN pan;
    uint8_t instrument_num;
    MUSINS *instrument;
    PCMSMP *sample;
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

#define _mixchn_get_type(o)         (o)->type
#define _mixchn_set_type(o, v)      _mixchn_get_type (o) = (v)
#define _mixchn_get_flags(o)        (o)->flags
#define _mixchn_set_flags(o, v)     _mixchn_get_flags (o) = (v)
#define _mixchn_is_enabled(o)       ((mixchn_get_flags (o) & MIXCHNFL_ENABLED) != 0)
#define _mixchn_is_playing(o)       ((mixchn_get_flags (o) & MIXCHNFL_PLAYING) != 0)
#define _mixchn_is_mixing(o)        ((mixchn_get_flags (o) & MIXCHNFL_MIXING) != 0)
#define _mixchn_get_pan(o)          (o)->pan
#define _mixchn_set_pan(o, v)       _mixchn_get_pan (o) = (v)
#define _mixchn_get_instrument_num(o)       (o)->instrument_num
#define _mixchn_set_instrument_num(o, v)    _mixchn_get_instrument_num (o) = (v)
#define _mixchn_get_instrument(o)           (o)->instrument
#define _mixchn_set_instrument(o, v)        _mixchn_get_instrument (o) = (v)
#define _mixchn_get_sample(o)       (o)->sample
#define _mixchn_set_sample(o, v)    _mixchn_get_sample (o) = (v)

void     __far mixchn_init (MIXCHN *self);
#define        mixchn_set_type(o, v)   _mixchn_set_type (o, v)
#define        mixchn_get_type(o)      _mixchn_get_type (o)
#define        mixchn_set_flags(o, v)   _mixchn_set_flags (o, v)
#define        mixchn_get_flags(o)      _mixchn_get_flags (o)
void     __far mixchn_set_enabled (MIXCHN *self, bool value);
#define        mixchn_is_enabled(o)     _mixchn_is_enabled (o)
void     __far mixchn_set_playing (MIXCHN *self, bool value);
#define        mixchn_is_playing(o)     _mixchn_is_playing (o)
void     __far mixchn_set_mixing (MIXCHN *self, bool value);
#define        mixchn_is_mixing(o)      _mixchn_is_mixing (o)
#define        mixchn_set_pan(o, v)     _mixchn_set_pan (o, v)
#define        mixchn_get_pan(o)        _mixchn_get_pan (o)
#define        mixchn_set_instrument_num(o, v)  _mixchn_set_instrument_num (o, v)
#define        mixchn_get_instrument_num(o)     _mixchn_get_instrument_num (o)
#define        mixchn_set_instrument(o, v)      _mixchn_set_instrument (o, v)
#define        mixchn_get_instrument(o)         _mixchn_get_instrument (o)
#define        mixchn_set_sample(o, v)  _mixchn_set_sample (o, v)
#define        mixchn_get_sample(o)     _mixchn_get_sample (o)
void     __far mixchn_set_sample_volume (MIXCHN *self, int16_t vol);
uint8_t  __far mixchn_get_sample_volume (MIXCHN *self);
void     __far mixchn_set_sample_period_limits (MIXCHN *self, uint16_t rate, bool amiga);
uint16_t __far mixchn_check_sample_period (MIXCHN *self, uint32_t value);
void     __far mixchn_set_sample_period (MIXCHN *self, uint16_t value);
uint16_t __far mixchn_get_sample_period (MIXCHN *self);
void     __far mixchn_set_sample_step (MIXCHN *self, uint32_t value);
uint32_t __far mixchn_get_sample_step (MIXCHN *self);
void     __far mixchn_setup_sample_period (MIXCHN *self, uint32_t value);
void     __far mixchn_set_command (MIXCHN *self, uint8_t value);
uint8_t  __far mixchn_get_command (MIXCHN *self);
void     __far mixchn_set_sub_command (MIXCHN *self, uint8_t value);
uint8_t  __far mixchn_get_sub_command (MIXCHN *self);
void     __far mixchn_set_command_parameter (MIXCHN *self, uint8_t value);
uint8_t  __far mixchn_get_command_parameter (MIXCHN *self);
void     __far mixchn_reset_wave_tables (MIXCHN *self);
void     __far mixchn_free (MIXCHN *self);

void     __far chn_setupInstrument (MIXCHN *chn, uint8_t insNum);
uint16_t __far chn_calcNotePeriod (MIXCHN *chn, uint32_t rate, uint8_t note);
uint32_t __far chn_calcNoteStep (MIXCHN *chn, uint32_t rate, uint8_t note);
void     __far chn_setupNote (MIXCHN *chn, uint8_t note, bool keep);

/*** Mixing channels list ***/

/* Flags */

typedef uint16_t mixing_channels_list_flags_t;
typedef mixing_channels_list_flags_t MIXCHNLFLAGS;

/* Structure */

#pragma pack(push, 1);
typedef struct mixing_channels_list_t
{
    MIXCHNLFLAGS flags;
    DYNARR list;
};
#pragma pack(pop);

typedef struct mixing_channels_list_t MIXCHNLIST;

/* Methods */

MIXCHNLFLAGS __far __mixchnl_set_flags (MIXCHNLFLAGS _flags, MIXCHNLFLAGS _mask, MIXCHNLFLAGS _set, bool raise);

void        __far mixchnl_init (MIXCHNLIST *self);
MIXCHN     *__far mixchnl_get (MIXCHNLIST *self, uint16_t index);
bool        __far mixchnl_set_count (MIXCHNLIST *self, uint16_t value);
uint16_t    __far mixchnl_get_count (MIXCHNLIST *self);
void        __far mixchnl_free (MIXCHNLIST *self);

/*** Variables ***/

extern MIXCHNLIST *mod_Channels;

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux mixchn_init "*";
#pragma aux mixchn_set_enabled "*";
#pragma aux mixchn_set_playing "*";
#pragma aux mixchn_set_mixing "*";
#pragma aux mixchn_set_instrument "*";
#pragma aux mixchn_get_instrument "*";
#pragma aux mixchn_set_sample_volume "*";
#pragma aux mixchn_get_sample_volume "*";
#pragma aux mixchn_set_sample_period_limits "*";
#pragma aux mixchn_check_sample_period "*";
#pragma aux mixchn_set_sample_period "*";
#pragma aux mixchn_get_sample_period "*";
#pragma aux mixchn_set_sample_step "*";
#pragma aux mixchn_get_sample_step "*";
#pragma aux mixchn_setup_sample_period "*";
#pragma aux mixchn_set_command "*";
#pragma aux mixchn_get_command "*";
#pragma aux mixchn_set_sub_command "*";
#pragma aux mixchn_get_sub_command "*";
#pragma aux mixchn_set_command_parameter "*";
#pragma aux mixchn_get_command_parameter "*";
#pragma aux mixchn_reset_wave_tables "*";
#pragma aux mixchn_free "*";

#pragma aux chn_setupInstrument "*";
#pragma aux chn_calcNotePeriod "*";
#pragma aux chn_calcNoteStep "*";
#pragma aux chn_setupNote "*";

#pragma aux __mixchnl_set_flags "*";

#pragma aux mixchnl_init "*";
#pragma aux mixchnl_get "*";
#pragma aux mixchnl_set_count "*";
#pragma aux mixchnl_get_count "*";
#pragma aux mixchnl_free "*";

#pragma aux mod_Channels "*";

#endif  /* __WATCOMC__ */

#endif  /* MIXCHN_H */
