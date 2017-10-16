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

#define MAX_PATTERNS 100
    /* 0..99 patterns */

typedef struct musPatternsList_t
{
    struct _dynarr_t list;
    uint16_t patLength; /* length of one pattern */
    bool useEM;         /* patterns in EM */
    EMSHDL handle;      /* handle to access EM for patterns */
    uint8_t patPerPage; /* count of patterns per page (<64!!!) */
};
typedef struct musPatternsList_t MUSPATLIST;

extern MUSPATLIST *PUBLIC_DATA mod_Patterns;

MUSPATLIST *PUBLIC_CODE patList_new(void);
void        PUBLIC_CODE patList_clear(MUSPATLIST *self);
void        PUBLIC_CODE patList_delete(MUSPATLIST **self);
void        PUBLIC_CODE patList_set(MUSPATLIST *self, uint16_t index, MUSPAT *pat);
MUSPAT     *PUBLIC_CODE patList_get(MUSPATLIST *self, uint16_t index);
bool        PUBLIC_CODE patList_set_count(MUSPATLIST *self, uint16_t count);
uint16_t    PUBLIC_CODE patList_get_count(MUSPATLIST *self);
void        PUBLIC_CODE patListSetPatLength(MUSPATLIST *self, uint16_t value);
uint16_t    PUBLIC_CODE patListGetPatLength(MUSPATLIST *self);
void        PUBLIC_CODE patListSetUseEM(MUSPATLIST *self, bool value);
bool        PUBLIC_CODE patListIsInEM(MUSPATLIST *self);
void        PUBLIC_CODE patListSetHandle(MUSPATLIST *self, EMSHDL value);
EMSHDL      PUBLIC_CODE patListGetHandle(MUSPATLIST *self);
void        PUBLIC_CODE patListSetHandleName(MUSPATLIST *self);
void        PUBLIC_CODE patListSetPatPerPage(MUSPATLIST *self, uint8_t value);
uint8_t     PUBLIC_CODE patListGetPatPerPage(MUSPATLIST *self);
uint32_t    PUBLIC_CODE patListGetUsedEM(MUSPATLIST *self);
void        PUBLIC_CODE patListFree(MUSPATLIST *self);

#endif  /* MUSPAT_H */
