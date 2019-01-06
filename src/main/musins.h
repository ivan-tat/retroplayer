/* musins.h -- declarations for musins.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MUSINS_H
#define MUSINS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "dos/ems.h"
#include "dynarray.h"
#include "main/pcmsmp.h"

/*** Musical instrument ***/

typedef uint8_t music_instrument_type_t;
typedef music_instrument_type_t MUSINSTYPE;

#define MUSINST_EMPTY 0
#define MUSINST_PCM   1
#define MUSINST_ADLIB 2

#define MUSINS_TITLE_LEN 28

#pragma pack(push, 1);
typedef struct instrument_t
{
    MUSINSTYPE  type;
    uint8_t     volume;
    void       *link;
    uint8_t     title[MUSINS_TITLE_LEN];
};
#pragma pack(pop);
typedef struct instrument_t MUSINS;

#define _musins_get_type(o)         (o)->type
#define _musins_set_type(o, v)      _musins_get_type (o) = (v)
#define _musins_get_volume(o)       (o)->volume
#define _musins_set_volume(o, v)    _musins_get_volume (o) = (v)
#define _musins_get_link(o)         (o)->link
#define _musins_get_sample(o)       (PCMSMP *) _musins_get_link (o)
#define _musins_set_sample(o, v)    _musins_get_link (o) = (void *) (v)
#define _musins_get_title(o)        (o)->title

void __far musins_init (MUSINS *self);
#define    musins_set_type(o, v)    _musins_set_type (o, v)
#define    musins_get_type(o)       _musins_get_type (o)
#define    musins_set_volume(o, v)  _musins_set_volume (o, v)
#define    musins_get_volume(o)     _musins_get_volume (o)
#define    musins_set_sample(o, v)  _musins_set_sample (o, v)
#define    musins_get_sample(o)     _musins_get_sample (o)
void __far musins_set_title (MUSINS *self, char *value);
#define    musins_get_title(o)      _musins_get_title (o)
void __far musins_free (MUSINS *self);

/*** Musical instruments list ***/

typedef struct music_instruments_list_t
{
    DYNARR list;
};
typedef struct music_instruments_list_t MUSINSLIST;

#define _musinsl_get_list(o)        & ((o)->list)
#define _musinsl_set_count(o, v)    dynarr_set_size (_musinsl_get_list (o), v)
#define _musinsl_get_count(o)       dynarr_get_size (_musinsl_get_list (o))
#define _musinsl_set(o, i, v)       dynarr_set_item (_musinsl_get_list (o), i, v)
#define _musinsl_get(o, i)          dynarr_get_item (_musinsl_get_list (o), i)

void __far musinsl_init (MUSINSLIST *self);
#define    musinsl_set_count(o, v)  _musinsl_set_count (o, v)
#define    musinsl_get_count(o)     _musinsl_get_count (o)
#define    musinsl_set(o, i, v)     _musinsl_set (o, i, v)
#define    musinsl_get(o, i)        _musinsl_get (o, i)
void __far musinsl_free (MUSINSLIST *self);

/*** Debug ***/

#if DEBUG == 1

void __far DEBUG_dump_instrument_info (MUSINS *self, uint8_t index, PCMSMPLIST *samples);

#else

#define DEBUG_dump_instrument_info(self, index, samples)

#endif  // DEBUG

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux musins_init "*";
#pragma aux musins_set_title "*";
#pragma aux musins_free "*";

#pragma aux DEBUG_dump_instrument_info "*";

#pragma aux musinsl_init "*";
#pragma aux musinsl_free "*";

#endif  /* __WATCOMC__ */

#endif  /* MUSINS_H */
