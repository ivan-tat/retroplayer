/* muspat.h -- declarations for muspat.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef MUSPAT_H
#define MUSPAT_H 1

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

/*** Music pattern ***/

typedef uint16_t music_pattern_flags_t;
typedef music_pattern_flags_t MUSPATFLAGS;

#define MUSPATFL_EM     (1<<0)  /* data is in EM */
#define MUSPATFL_OWNHDL (1<<1)  /* has own EM handle, needs to be freed when done */

#pragma pack(push, 1);
typedef struct music_pattern_t
{
    MUSPATFLAGS flags;
    uint8_t channels;
    uint8_t rows;
    uint16_t size;
    uint16_t data_off;
    uint16_t data_seg;  /* DOS segment or EM page */
    EMSHDL handle;
};
#pragma pack(pop);
typedef struct music_pattern_t MUSPAT;

void     __far muspat_init (MUSPAT *self);
void     __far muspat_set_EM_data (MUSPAT *self, bool value);
bool     __far muspat_is_EM_data (MUSPAT *self);
void     __far muspat_set_own_EM_handle (MUSPAT *self, bool value);
bool     __far muspat_is_own_EM_handle (MUSPAT *self);
void     __far muspat_set_channels (MUSPAT *self, uint8_t value);
uint8_t  __far muspat_get_channels (MUSPAT *self);
void     __far muspat_set_rows (MUSPAT *self, uint8_t value);
uint8_t  __far muspat_get_rows (MUSPAT *self);
void     __far muspat_set_size (MUSPAT *self, uint16_t value);
uint16_t __far muspat_get_size (MUSPAT *self);
void     __far muspat_set_data (MUSPAT *self, void *value);
void     __far muspat_set_EM_data_handle (MUSPAT *self, EMSHDL value);
EMSHDL   __far muspat_get_EM_data_handle (MUSPAT *self);
void     __far muspat_set_EM_data_page (MUSPAT *self, uint16_t value);
uint16_t __far muspat_get_EM_data_page (MUSPAT *self);
void     __far muspat_set_EM_data_offset (MUSPAT *self, uint16_t value);
uint16_t __far muspat_get_EM_data_offset (MUSPAT *self);
void    *__far muspat_get_data (MUSPAT *self);
void    *__far muspat_map_EM_data (MUSPAT *self);
void     __far muspat_free (MUSPAT *self);

/*** Patterns list ***/

typedef uint16_t music_patterns_list_flags_t;
typedef music_patterns_list_flags_t MUSPATLFLAGS;

#define MUSPATLFL_EM     (1<<0) /* data is in EM */
#define MUSPATLFL_OWNHDL (1<<1) /* has own EM handle, needs to be freed when done */

typedef struct music_patterns_list_t
{
    MUSPATLFLAGS flags;
    DYNARR list;
    EMSHDL handle;
};
typedef struct music_patterns_list_t MUSPATLIST;

void        __far muspatl_init (MUSPATLIST *self);
void        __far muspatl_set (MUSPATLIST *self, uint16_t index, MUSPAT *item);
MUSPAT     *__far muspatl_get (MUSPATLIST *self, uint16_t index);
bool        __far muspatl_set_count (MUSPATLIST *self, uint16_t value);
uint16_t    __far muspatl_get_count (MUSPATLIST *self);
void        __far muspatl_set_EM_data (MUSPATLIST *self, bool value);
bool        __far muspatl_is_EM_data (MUSPATLIST *self);
void        __far muspatl_set_own_EM_handle (MUSPATLIST *self, bool value);
bool        __far muspatl_is_own_EM_handle (MUSPATLIST *self);
void        __far muspatl_set_EM_handle (MUSPATLIST *self, EMSHDL value);
EMSHDL      __far muspatl_get_EM_handle (MUSPATLIST *self);
void        __far muspatl_set_EM_handle_name (MUSPATLIST *self);
uint32_t    __far muspatl_get_used_EM (MUSPATLIST *self);
void        __far muspatl_free (MUSPATLIST *self);

/*** Variables ***/

extern MUSPATLIST *mod_Patterns;

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux muspat_init "*";
#pragma aux muspat_set_EM_data "*";
#pragma aux muspat_is_EM_data "*";
#pragma aux muspat_set_own_EM_handle "*";
#pragma aux muspat_is_own_EM_handle "*";
#pragma aux muspat_set_channels "*";
#pragma aux muspat_get_channels "*";
#pragma aux muspat_set_rows "*";
#pragma aux muspat_get_rows "*";
#pragma aux muspat_set_size "*";
#pragma aux muspat_get_size "*";
#pragma aux muspat_set_data "*";
#pragma aux muspat_set_EM_data_handle "*";
#pragma aux muspat_get_EM_data_handle "*";
#pragma aux muspat_set_EM_data_page "*";
#pragma aux muspat_get_EM_data_page "*";
#pragma aux muspat_set_EM_data_offset "*";
#pragma aux muspat_get_EM_data_offset "*";
#pragma aux muspat_get_data "*";
#pragma aux muspat_map_EM_data "*";
#pragma aux muspat_free "*";

#pragma aux muspatl_init "*";
#pragma aux muspatl_set "*";
#pragma aux muspatl_get "*";
#pragma aux muspatl_set_count "*";
#pragma aux muspatl_get_count "*";
#pragma aux muspatl_set_EM_data "*";
#pragma aux muspatl_is_EM_data "*";
#pragma aux muspatl_set_own_EM_handle "*";
#pragma aux muspatl_is_own_EM_handle "*";
#pragma aux muspatl_set_EM_handle "*";
#pragma aux muspatl_get_EM_handle "*";
#pragma aux muspatl_set_EM_handle_name "*";
#pragma aux muspatl_get_used_EM "*";
#pragma aux muspatl_free "*";

#pragma aux mod_Patterns "*";

#endif  /* __WATCOMC__ */

#endif  /* MUSPAT_H */
