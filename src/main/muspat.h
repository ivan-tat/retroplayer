/* muspat.h -- declarations for musical pattern handling library.

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

//MUSPAT  *PUBLIC_CODE muspat_new(void);
void     PUBLIC_CODE muspat_clear(MUSPAT *self);
//void     PUBLIC_CODE muspat_delete(MUSPAT **self);
void     PUBLIC_CODE muspat_set_EM_data(MUSPAT *self, bool value);
bool     PUBLIC_CODE muspat_is_EM_data(MUSPAT *self);
void     PUBLIC_CODE muspat_set_own_EM_handle(MUSPAT *self, bool value);
bool     PUBLIC_CODE muspat_is_own_EM_handle(MUSPAT *self);
void     PUBLIC_CODE muspat_set_channels(MUSPAT *self, uint8_t value);
uint8_t  PUBLIC_CODE muspat_get_channels(MUSPAT *self);
void     PUBLIC_CODE muspat_set_rows(MUSPAT *self, uint8_t value);
uint8_t  PUBLIC_CODE muspat_get_rows(MUSPAT *self);
void     PUBLIC_CODE muspat_set_size(MUSPAT *self, uint16_t value);
uint16_t PUBLIC_CODE muspat_get_size(MUSPAT *self);
void     PUBLIC_CODE muspat_set_data(MUSPAT *self, void *value);
void     PUBLIC_CODE muspat_set_EM_data_handle(MUSPAT *self, EMSHDL value);
EMSHDL   PUBLIC_CODE muspat_get_EM_data_handle(MUSPAT *self);
void     PUBLIC_CODE muspat_set_EM_data_page(MUSPAT *self, uint16_t value);
uint16_t PUBLIC_CODE muspat_get_EM_data_page(MUSPAT *self);
void     PUBLIC_CODE muspat_set_EM_data_offset(MUSPAT *self, uint16_t value);
uint16_t PUBLIC_CODE muspat_get_EM_data_offset(MUSPAT *self);
void    *PUBLIC_CODE muspat_get_data(MUSPAT *self);
void    *PUBLIC_CODE muspat_map_EM_data(MUSPAT *self);
void     PUBLIC_CODE muspat_free(MUSPAT *self);

/*** Patterns list ***/

typedef uint16_t music_patterns_list_flags_t;
typedef music_patterns_list_flags_t MUSPATLFLAGS;

#define MUSPATLFL_EM     (1<<0) /* data is in EM */
#define MUSPATLFL_OWNHDL (1<<1) /* has own EM handle, needs to be freed when done */

typedef struct music_patterns_list_t
{
    MUSPATLFLAGS flags;
    struct _dynarr_t list;
    EMSHDL handle;
};
typedef struct music_patterns_list_t MUSPATLIST;

MUSPATLIST *PUBLIC_CODE muspatl_new(void);
void        PUBLIC_CODE muspatl_clear(MUSPATLIST *self);
void        PUBLIC_CODE muspatl_delete(MUSPATLIST **self);
void        PUBLIC_CODE muspatl_set(MUSPATLIST *self, uint16_t index, MUSPAT *item);
MUSPAT     *PUBLIC_CODE muspatl_get(MUSPATLIST *self, uint16_t index);
bool        PUBLIC_CODE muspatl_set_count(MUSPATLIST *self, uint16_t value);
uint16_t    PUBLIC_CODE muspatl_get_count(MUSPATLIST *self);
void        PUBLIC_CODE muspatl_set_EM_data(MUSPATLIST *self, bool value);
bool        PUBLIC_CODE muspatl_is_EM_data(MUSPATLIST *self);
void        PUBLIC_CODE muspatl_set_own_EM_handle(MUSPATLIST *self, bool value);
bool        PUBLIC_CODE muspatl_is_own_EM_handle(MUSPATLIST *self);
void        PUBLIC_CODE muspatl_set_EM_handle(MUSPATLIST *self, EMSHDL value);
EMSHDL      PUBLIC_CODE muspatl_get_EM_handle(MUSPATLIST *self);
void        PUBLIC_CODE muspatl_set_EM_handle_name(MUSPATLIST *self);
uint32_t    PUBLIC_CODE muspatl_get_used_EM(MUSPATLIST *self);
void        PUBLIC_CODE muspatl_free(MUSPATLIST *self);

/* Variables */

extern MUSPATLIST *PUBLIC_DATA mod_Patterns;

#endif  /* MUSPAT_H */
