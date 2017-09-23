/* ems.h -- declarations for ems.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef EMS_H
#define EMS_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

/* types */

typedef struct emsVersion_t
{
    uint8_t Lo, Hi;
};
typedef struct emsVersion_t EMSVER;

typedef uint16_t emsError_t;
typedef emsError_t EMSERR;

typedef uint16_t emsHandle_t;
typedef emsHandle_t EMSHDL;

typedef uint8_t emsHandleName_t[8];
typedef emsHandleName_t EMSNAME;

/* external variables */

extern bool     PUBLIC_DATA emsInstalled;
extern EMSERR   PUBLIC_DATA emsEC;
extern EMSVER   PUBLIC_DATA emsVersion;
extern uint16_t PUBLIC_DATA emsFrameSeg;    /* real memory segment for first page */
extern void    *PUBLIC_DATA emsFramePtr;    /* real memory pointer for first page */

/* public functions */

#define _emsCalcPagesCount(size) (unsigned int)(((unsigned long)(size + 0x3fff) << 2) >> 16)

#ifdef USE_INTRINSICS

#define emsCalcPagesCount(size) _emsCalcPagesCount(size)

#else

uint16_t emsCalcPagesCount(uint32_t size);

#endif

bool        PUBLIC_CODE emsIsInstalled(void);
const char *PUBLIC_CODE emsGetErrorMsg(void);
bool        PUBLIC_CODE emsGetVersion(void);
uint16_t    PUBLIC_CODE emsGetFrameSeg(void);
uint16_t    PUBLIC_CODE emsGetFreePagesCount(void);
EMSHDL      PUBLIC_CODE emsAlloc(uint16_t pages);
bool        PUBLIC_CODE emsResize(EMSHDL handle, uint16_t pages);
bool        PUBLIC_CODE emsFree(EMSHDL handle);
bool        PUBLIC_CODE emsMap(EMSHDL handle, uint16_t logPage, uint8_t physPage);
bool        PUBLIC_CODE emsSaveMap(EMSHDL handle);
bool        PUBLIC_CODE emsRestoreMap(EMSHDL handle);
uint16_t    PUBLIC_CODE emsGetHandleSize(EMSHDL handle);
bool        PUBLIC_CODE emsSetHandleName(EMSHDL handle, EMSNAME *name);

/*** Initialization ***/

DECLARE_REGISTRATION(ems)

#endif  /* EMS_H */
