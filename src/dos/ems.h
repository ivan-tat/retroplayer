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

#define E_EMS_SUCCESS 0

typedef uint16_t emsHandle_t;
typedef emsHandle_t EMSHDL;

#define EMSBADHDL 0xffff

typedef uint8_t emsHandleName_t[8];
typedef emsHandleName_t EMSNAME;

/* external variables */

extern bool     emsInstalled;
extern EMSERR   emsEC;
extern EMSVER   emsVersion;
extern uint16_t emsFrameSeg;    /* real memory segment for first page */
extern void    *emsFramePtr;    /* real memory pointer for first page */

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

DECLARE_REGISTRATION (ems)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux emsInstalled "*";
#pragma aux emsEC "*";
#pragma aux emsVersion "*";
#pragma aux emsFrameSeg "*";
#pragma aux emsFramePtr "*";

#ifndef USE_INTRINSICS
#pragma aux emsCalcPagesCount "*";
#endif

#pragma aux emsIsInstalled "*";
#pragma aux emsGetErrorMsg "*";
#pragma aux emsGetVersion "*";
#pragma aux emsGetFrameSeg "*";
#pragma aux emsGetFreePagesCount "*";
#pragma aux emsAlloc "*";
#pragma aux emsResize "*";
#pragma aux emsFree "*";
#pragma aux emsMap "*";
#pragma aux emsSaveMap "*";
#pragma aux emsRestoreMap "*";
#pragma aux emsGetHandleSize "*";
#pragma aux emsSetHandleName "*";

#pragma aux register_ems "*";
#pragma aux unregister_ems "*";

#endif  /* __WATCOMC__ */

#endif  /* EMS_H */
