/* emstool.h -- declarations for emstool.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef EMSTOOL_H
#define EMSTOOL_H 1

#ifdef __WATCOMC__
#pragma once
#include <stdbool.h>
#include <stdint.h>
#endif

#include "../pascal/pascal.h"

/* types */

typedef struct EMMVersion_t {
    uint8_t Lo, Hi;
};
typedef uint16_t EMMError_t;
typedef uint16_t EMMHandle_t;
typedef uint8_t EMMHandleName_t[8];
typedef struct EMMHandleEntry_t {
    EMMHandle_t handle;
    struct EMMHandleEntry_t *next;
};

/* external variables */

extern bool     PUBLIC_DATA EmsInstalled;
extern int16_t  PUBLIC_DATA EmsEC;
extern struct EMMVersion_t PUBLIC_DATA EmsVersion;
extern uint16_t PUBLIC_DATA FrameSEG[4];
extern void    *PUBLIC_DATA FramePTR[4];

/* public functions */

bool        PUBLIC_CODE CheckEMM(void);
const char *PUBLIC_CODE GetEMMErrorMsg(uint16_t err);
bool        PUBLIC_CODE GetEMMVersion(void);
uint16_t    PUBLIC_CODE GetEMMFrameSeg(void);
uint16_t    PUBLIC_CODE EmsFreePages(void);
EMMHandle_t PUBLIC_CODE EmsAlloc(uint16_t pages);
bool        PUBLIC_CODE EmsFree(EMMHandle_t handle);
bool        PUBLIC_CODE EmsMap(EMMHandle_t handle, uint16_t logPage, uint8_t physPage);
bool        PUBLIC_CODE EmsSaveMap(EMMHandle_t handle);
bool        PUBLIC_CODE EmsRestoreMap(EMMHandle_t handle);
uint16_t    PUBLIC_CODE EmsGetHandleSize(EMMHandle_t handle);
bool        PUBLIC_CODE EmsSetHandleName(EMMHandle_t handle, EMMHandleName_t *name);

/*** Initialization ***/

DECLARE_REGISTRATION(emstool)

#endif  /* EMSTOOL_H */
