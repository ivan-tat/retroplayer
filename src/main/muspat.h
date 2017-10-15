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
#include "dynarray.h"

/*** Pattern ***/

typedef struct pattern_t {
    uint16_t data_seg;
        /* segment for every pattern: 0xC000 + ((part & 63) << 8) + page */
};
typedef struct pattern_t MUSPAT;

#define _patIsDataInEM(o)              (o->data_seg >= 0xc000)
#define _patGetDataEMPage(o)           (o->data_seg & 0x00ff)
#define _patGetDataEMPart(o)           ((o->data_seg >> 8) & 0x3f)
#define _patSetDataInEM(o, page, part) { o->data_seg = 0xc000 + (((part) & 0x3f) << 8) + (page); }
#define _patGetDataInEM(o, length)     (MK_FP(emsFrameSeg, _patGetDataEMPart(o) * (length)))

//MUSPAT *PUBLIC_CODE pat_new(void);
void    PUBLIC_CODE pat_clear(MUSPAT *pat);
//void    PUBLIC_CODE pat_delete(MUSPAT **pat);
void    PUBLIC_CODE patSetData(MUSPAT *pat, void *p);
void    PUBLIC_CODE patSetDataInEM(MUSPAT *pat, uint8_t page, uint8_t part);
bool    PUBLIC_CODE patIsDataInEM(MUSPAT *pat);
void   *PUBLIC_CODE patGetData(MUSPAT *pat);
uint8_t PUBLIC_CODE patGetDataEMPage(MUSPAT *pat);
uint8_t PUBLIC_CODE patGetDataEMPart(MUSPAT *pat);
void   *PUBLIC_CODE patMapData(MUSPAT *pat);
void    PUBLIC_CODE patFree(MUSPAT *pat);
//void    PUBLIC_CODE patInit(MUSPAT *pat);
//void    PUBLIC_CODE patDone(MUSPAT *pat);

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
