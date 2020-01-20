/* ems.h -- declarations for ems.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _EMS_H_INCLUDED
#define _EMS_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"

/* types */

#define EM_PAGE_SIZE (16 * 1024)

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

#if USE_INTRINSICS == 1

#define emsCalcPagesCount(size) _emsCalcPagesCount(size)

#else  /* USE_INTRINSICS != 1 */

uint16_t emsCalcPagesCount(uint32_t size);

#endif  /* USE_INTRINSICS != 1 */

bool        __far emsIsInstalled (void);
const char *__far emsGetErrorMsg (void);
bool        __far emsGetVersion (void);
uint16_t    __far emsGetFrameSeg (void);
uint16_t    __far emsGetFreePagesCount (void);
EMSHDL      __far emsAlloc (uint16_t pages);
bool        __far emsResize (EMSHDL handle, uint16_t pages);
bool        __far emsFree (EMSHDL handle);
bool        __far emsMap (EMSHDL handle, uint16_t logPage, uint8_t physPage);
bool        __far emsSaveMap (EMSHDL handle);
bool        __far emsRestoreMap (EMSHDL handle);
uint16_t    __far emsGetHandleSize (EMSHDL handle);
bool        __far emsSetHandleName (EMSHDL handle, EMSNAME *name);

/*** Initialization ***/

DECLARE_REGISTRATION (ems)

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux emsInstalled "*";
#pragma aux emsEC "*";
#pragma aux emsVersion "*";
#pragma aux emsFrameSeg "*";
#pragma aux emsFramePtr "*";

#if USE_INTRINSICS != 1
#pragma aux emsCalcPagesCount "*";
#endif  /* USE_INTRINSICS != 1 */

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

#endif  /* !_EMS_H_INCLUDED */
