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

/*** Pattern ***/

typedef struct pattern_t {
    uint16_t data_seg;
        /* segment for every pattern: 0xC000 + ((part & 63) << 8) + page */
};
typedef struct pattern_t MUSPAT;

#define isPatternDataInEM(seg)                ((seg) >= 0xc000)
#define getPatternDataLogPageInEM(seg)        ((seg) & 0x00ff)
#define getPatternDataPartInEM(seg)           (((seg) >> 8) & 0x3f)
#define getPatternDataOffsetInEM(seg, length) (getPatternDataPartInEM(seg) * (length))
#define setPatternDataInEM(logpage, part)     (0xc000 + (((part) & 0x3f) << 8) + (logpage))
#define getPatternDataInEM(seg, length)       (MK_FP(emsFrameSeg, getPatternDataOffsetInEM((seg), (length))))

//MUSPAT *PUBLIC_CODE pat_new(void);
void    PUBLIC_CODE pat_clear(MUSPAT *pat);
//void    PUBLIC_CODE pat_delete(MUSPAT **pat);
void    PUBLIC_CODE patSetData(MUSPAT *pat, void *p);
void    PUBLIC_CODE patSetDataInEM(MUSPAT *pat, uint8_t logpage, uint8_t part);
bool    PUBLIC_CODE patIsDataInEM(MUSPAT *pat);
void   *PUBLIC_CODE patGetData(MUSPAT *pat);
uint8_t PUBLIC_CODE patGetDataLogPageInEM(MUSPAT *pat);
uint8_t PUBLIC_CODE patGetDataPartInEM(MUSPAT *pat);
void   *PUBLIC_CODE patMapData(MUSPAT *pat);
void    PUBLIC_CODE patFree(MUSPAT *pat);
//void    PUBLIC_CODE patInit(MUSPAT *pat);
//void    PUBLIC_CODE patDone(MUSPAT *pat);

/*** Patterns list ***/

#define MAX_PATTERNS 100
    /* 0..99 patterns */

typedef struct pattern_t patternsList_t[MAX_PATTERNS];

extern patternsList_t PUBLIC_DATA Pattern;
extern uint16_t PUBLIC_DATA patListCount;
extern uint16_t PUBLIC_DATA patListPatLength;   /* length of one pattern */
extern bool     PUBLIC_DATA patListUseEM;       /* patterns in EM */
extern uint16_t PUBLIC_DATA patListEMHandle;    /* handle to access EM for patterns */
extern uint8_t  PUBLIC_DATA patListPatPerEMPage;  /* count of patterns per page (<64!!!) */

void     PUBLIC_CODE patList_set(int16_t index, MUSPAT *pat);
MUSPAT  *PUBLIC_CODE patList_get(int16_t index);
uint32_t PUBLIC_CODE patListGetUsedEM(void);
void     PUBLIC_CODE patListFree(void);
void     PUBLIC_CODE patListInit(void);
void     PUBLIC_CODE patListDone(void);

#endif  /* MUSPAT_H */
