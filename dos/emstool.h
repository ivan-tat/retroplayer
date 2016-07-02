/* Declarations for emstool.

   This is free and unencumbered software released into the public domain */

#ifndef _EMSTOOL_H
#define _EMSTOOL_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

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

extern bool __pascal EmsInstalled;
extern int16_t __pascal EmsEC;
extern struct EMMVersion_t __pascal EmsVersion;
extern uint16_t __pascal FrameSEG[4];
extern void * __pascal FramePTR[4];

/* public functions */

bool __far __pascal CheckEMM( void );
const char * __far __pascal GetEMMErrorMsg( uint16_t err );
bool __far __pascal GetEMMVersion( void );
uint16_t __far __pascal GetEMMFrameSeg( void );
uint16_t __far __pascal EmsFreePages( void );
EMMHandle_t __far __pascal EmsAlloc( uint16_t pages );
bool __far __pascal EmsFree( EMMHandle_t handle );
bool __far __pascal EmsMap( EMMHandle_t handle, uint16_t logPage, uint8_t physPage );
bool __far __pascal EmsSaveMap( EMMHandle_t handle );
bool __far __pascal EmsRestoreMap( EMMHandle_t handle );
void __far __pascal EMMInit( void );
void __far __pascal EMMDone( void );

#endif /* _EMSTOOL_H */
