/* musmod.h -- declarations for musmod.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MUSMOD_H
#define MUSMOD_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/string.h"
#include "main/pcmsmp.h"
#include "main/musins.h"
#include "main/muspat.h"

/* Limits */

#define MUSMOD_TITLE_LEN    28  // including trailing zero
#define MUSMOD_FORMAT_LEN   32  // including trailing zero
#define MUSMOD_CHANNELS_MAX 32
#define MUSMOD_ORDER_LEN    100 // 0..99 entries

/* Channel panning */

typedef uint8_t musmod_channel_pan_t;
typedef musmod_channel_pan_t MUSMODCHNPAN;

#define MUSMODCHNPANFL_PAN_MASK 0x7f
#define MUSMODCHNPANFL_ENABLED  0x80

#define MUSMODCHNPAN_LEFT   0
#define MUSMODCHNPAN_CENTER 32
#define MUSMODCHNPAN_RIGHT  64

/* Channel */

#pragma pack(push, 1);
typedef struct musmod_channel_t
{
    MUSMODCHNPAN pan;
};
#pragma pack(pop);
typedef struct musmod_channel_t MUSMODCHN;

/* Song arrangement */

typedef uint8_t musmod_order_entry_t;
typedef musmod_order_entry_t musmod_order_t[MUSMOD_ORDER_LEN];
typedef musmod_order_t MUSMODORDER;

/* Flags */

typedef uint16_t musmod_flags_t;
typedef musmod_flags_t MUSMODFLAGS;

#define MUSMODFL_LOADED         (1 << 0)
#define MUSMODFL_STEREO         (1 << 1)
#define MUSMODFL_AMIGA_LIMITS   (1 << 2)

/* Structure */

typedef struct music_module_t
{
    MUSMODFLAGS flags;
    char        title[MUSMOD_TITLE_LEN];
    char        format[MUSMOD_FORMAT_LEN];
    MUSMODCHN   channels[MUSMOD_CHANNELS_MAX];
    PCMSMPLIST  samples;
    /*
    MUSINSLIST  instruments;
    MUSPATLIST  patterns;
    MUSMODORDER order;
    */
    uint8_t     channels_count;
    uint8_t     order_length;
    /*
    uint16_t    order_start;
    uint8_t     global_volume;
    uint8_t     master_volume;
    */
    uint8_t     tempo;
    uint8_t     speed;
};
typedef struct music_module_t MUSMOD;

/* Methods */

MUSMODFLAGS __far __musmod_set_flags (MUSMODFLAGS _flags, MUSMODFLAGS _mask, MUSMODFLAGS _set, bool raise);

#define _musmod_get_flags(o)            (o)->flags
#define _musmod_set_flags(o, v)         _musmod_get_flags (o) = (v)
#define _musmod_set_loaded(o, v)        _musmod_set_flags (o, __musmod_set_flags (_musmod_get_flags (o), ~MUSMODFL_LOADED, MUSMODFL_LOADED, v))
#define _musmod_is_loaded(o)            ((_musmod_get_flags (o) & MUSMODFL_LOADED) != 0)
#define _musmod_set_stereo(o, v)        _musmod_set_flags (o, __musmod_set_flags (_musmod_get_flags (o), ~MUSMODFL_STEREO, MUSMODFL_STEREO, v))
#define _musmod_is_stereo(o)            ((_musmod_get_flags (o) & MUSMODFL_STEREO) != 0)
#define _musmod_set_amiga_limits(o, v)  _musmod_set_flags (o, __musmod_set_flags (_musmod_get_flags (o), ~MUSMODFL_AMIGA_LIMITS, MUSMODFL_AMIGA_LIMITS, v))
#define _musmod_is_amiga_limits(o)      ((_musmod_get_flags (o) & MUSMODFL_AMIGA_LIMITS) != 0)
#define _musmod_get_title(o)            (o)->title
#define _musmod_get_format(o)           (o)->format
#define _musmod_get_channels(o)         (o)->channels
#define _musmod_get_samples(o)          & ((o)->samples)
/*
#define _musmod_get_instruments(o)      & ((o)->instruments)
#define _musmod_get_patterns(o)         & ((o)->patterns)
#define _musmod_get_order(o)            (o)->order
*/
#define _musmod_get_channels_count(o)   (o)->channels_count
#define _musmod_set_channels_count(o, v) _musmod_get_channels_count (o) = (v)
#define _musmod_get_order_length(o)     (o)->order_length
#define _musmod_set_order_length(o, v)  _musmod_get_order_length (o) = (v)
/*
#define _musmod_get_order_start(o)      (o)->order_start
#define _musmod_set_order_start(o, v)   _musmod_get_order_start (o) = (v)
#define _musmod_get_global_volume(o)    (o)->global_volume
#define _musmod_set_global_volume(o, v) _musmod_get_global_volume (o) = (v)
#define _musmod_get_master_volume(o)    (o)->master_volume
#define _musmod_set_master_volume(o, v) _musmod_get_master_volume (o) = (v)
*/
#define _musmod_get_tempo(o)            (o)->tempo
#define _musmod_set_tempo(o, v)         _musmod_get_tempo(o) = (v)
#define _musmod_get_speed(o)            (o)->speed
#define _musmod_set_speed(o, v)         _musmod_get_speed(o) = (v)

void __far musmod_init (MUSMOD *self);
#define    musmod_set_flags(o, v)           _musmod_set_flags (o, v)
#define    musmod_get_flags(o)              _musmod_get_flags (o)
#define    musmod_set_loaded(o, v)          _musmod_set_loaded (o, v)
#define    musmod_is_loaded(o)              _musmod_is_loaded (o)
#define    musmod_set_stereo(o, v)          _musmod_set_stereo (o, v)
#define    musmod_is_stereo(o)              _musmod_is_stereo (o)
#define    musmod_set_amiga_limits(o, v)    _musmod_set_amiga_limits (o, v)
#define    musmod_is_amiga_limits(o)        _musmod_is_amiga_limits (o)
void __far musmod_set_title (MUSMOD *self, char *value);
#define    musmod_get_title(o)              (o) ? _musmod_get_title (o) : NULL
void __far musmod_set_format (MUSMOD *self, char *value);
#define    musmod_get_format(o)             (o) ? _musmod_get_format (o) : NULL
#define    musmod_get_channels(o)           _musmod_get_channels (o)
#define    musmod_get_channels_count(o)     _musmod_get_channels_count (o)
#define    musmod_set_channels_count(o, v)  _musmod_set_channels_count (o, v)
/*
#define    musmod_get_samples(o)            (o) ? _musmod_get_samples (o) : NULL
#define    musmod_get_instruments(o)        (o) ? _musmod_get_instruments (o) : NULL
#define    musmod_get_patterns(o)           (o) ? _musmod_get_patterns (o) : NULL
#define    musmod_get_order(o)              (o) ? _musmod_get_order (o) : NULL
*/
#define    musmod_get_order_length(o)       _musmod_get_order_length (o)
#define    musmod_set_order_length(o, v)    _musmod_set_order_length (o, v)
/*
#define    musmod_get_order_start(o)        _musmod_get_order_start (o)
#define    musmod_set_order_start(o, v)     _musmod_set_order_start (o, v)
#define    musmod_get_global_volume(o)      _musmod_get_global_volume (o)
#define    musmod_set_global_volume(o, v)   _musmod_set_global_volume (o, v)
#define    musmod_get_master_volume(o)      _musmod_get_master_volume (o)
#define    musmod_set_master_volume(o, v)   _musmod_set_master_volume (o, v)
*/
#define    musmod_get_tempo(o)              _musmod_get_tempo (o)
#define    musmod_set_tempo(o, v)           _musmod_set_tempo (o, v)
#define    musmod_get_speed(o)              _musmod_get_speed (o)
#define    musmod_set_speed(o, v)           _musmod_set_speed (o, v)
void __far musmod_free (MUSMOD *self);

/*** Variables ***/

extern MUSMOD *mod_Track;

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux __musmod_set_flags "*";

#pragma aux musmod_init "*";
#pragma aux musmod_set_title "*";
#pragma aux musmod_set_format "*";
#pragma aux musmod_free "*";

#pragma aux mod_Track "*";

#endif  /* __WATCOMC__ */

#endif  /* MUSMOD_H */
