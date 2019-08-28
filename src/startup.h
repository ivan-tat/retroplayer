/* startup.h -- declarations for startup.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef STARTUP_H
#define STARTUP_H

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>
#include "pascal.h"
#include "cc/dos/limits.h"

/* File I/O */

// File handle
#define cc_UnusedHandle 0

// File mode
#define cc_fmClosed 0xD7B0
#define cc_fmInput  0xD7B1
#define cc_fmOutput 0xD7B2
#define cc_fmInOut  0xD7B3

typedef struct _cc_iobuf_t;
typedef struct _cc_iobuf_t _cc_iobuf;

typedef int cc_inoutres_t;
typedef cc_inoutres_t inoutres_t;

typedef cc_inoutres_t __far _cc_iobuf_proc_t (_cc_iobuf *f);

#pragma pack(push, 1);
typedef struct _cc_iobuf_t {
    uint16_t handle;
    uint16_t mode;
    uint16_t rec_size;
    //char private_data[26];
    uint16_t unknown;
    uint16_t pos;
    uint16_t data_size;
    void *buffer;
    _cc_iobuf_proc_t *proc[4];
    char user_data[16];
    char name[cc_PathStr_size];
};
#pragma pack(pop);

// cc_InOutRes variable
#define EINOUTRES_SUCCESS 0
#define EINOUTRES_READ 100
#define EINOUTRES_WRITE 101
#define EINOUTRES_NOT_ASSIGNED 102
#define EINOUTRES_NOT_OPENED 103
#define EINOUTRES_NOT_INPUT 104
#define EINOUTRES_NOT_OUTPUT 105
#define EINOUTRES_NOT_NUMBER 106

/* Global variables */

extern uint16_t     _cc_psp;
extern uint16_t     _cc_argc;
extern const char **_cc_argv;
extern void __far  *cc_ErrorAddr;
extern void *(__far cc_ExitProc);
extern int16_t      cc_ExitCode;
extern inoutres_t   cc_InOutRes;
extern uint8_t      cc_Test8086;
extern _cc_iobuf    cc_Input;
extern _cc_iobuf    cc_Output;

/* Internal variables */
#define _CC_ATEXIT_MAX 32
extern void *__far _cc_ExitList[_CC_ATEXIT_MAX];
extern uint8_t _cc_ExitCount;

//void _cc_on_exit(void);   // internal

void __noreturn _cc_ExitWithError (int16_t status, void __far *addr);
void __noreturn _cc_Exit (int16_t status);

/* Arguments handling */

#if LINKER_TPC == 1
extern uint16_t __far __pascal pascal_paramcount(void);
extern void     __far __pascal pascal_paramstr(char *dest, uint8_t i);
extern void     __noreturn __far __pascal pascal_Halt (uint16_t status);
#endif

/* System unit */

/* (+|-)InOutRes(+|-): prefix "+" means IO state check is done on enter, postfix "+" - on exit. */

inoutres_t __far  FileWriteString (_cc_iobuf *f, char *str, uint16_t _n);   /* +InOutRes+ */
inoutres_t __far  FileWriteNewLine (_cc_iobuf *f);                          /* +InOutRes+ */
inoutres_t __far  FileFlushBuffer (_cc_iobuf *f);                           /* +InOutRes+ */
unsigned   __far  FileReadString (_cc_iobuf *f, char *dest, uint16_t max);  /* +InOutRes? */
inoutres_t __far  FileSkipToNextLine (_cc_iobuf *f);                        /* +InOutRes+ */
inoutres_t __far  FileWriteChar (_cc_iobuf *f, char _c, uint16_t _n);       /* +InOutRes+ */
int32_t    __far  FileReadNumber (_cc_iobuf *f);                            /* -InOutRes+ */
inoutres_t __far  FileWriteNumber (_cc_iobuf *f, uint32_t value, int n);    /* +InOutRes+ */

void __noreturn __far __stdcall _cc_local_int0 (void __far *addr, uint16_t flags);
void __noreturn __far __stdcall _cc_local_int23 (void __far *addr, uint16_t flags);

/* Application startup */

void _cc_startup(void);

/*** Aliases ***/

#define _psp _cc_psp
#define _argc _cc_argc
#define _argv _cc_argv

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux _cc_psp "*";
#pragma aux _cc_argc "*";
#pragma aux _cc_argv "*";
#pragma aux cc_ErrorAddr "*";
#pragma aux cc_ExitProc "*";
#pragma aux cc_ExitCode "*";
#pragma aux cc_InOutRes "*";
#pragma aux cc_Test8086 "*";
#pragma aux cc_Input "*";
#pragma aux cc_Output "*";

#pragma aux _cc_ExitList "*";
#pragma aux _cc_ExitCount "*";
//#pragma aux _cc_on_exit "*";
#pragma aux _cc_ExitWithError "*";
#pragma aux _cc_Exit "*";

#if LINKER_TPC == 1
#pragma aux pascal_paramcount "*" modify [    bx cx dx si di es ];
#pragma aux pascal_paramstr   "*" modify [ ax bx cx dx si di es ];
#pragma aux pascal_Halt       "*";
#endif  /* LINKER_TPC == 1 */

#pragma aux FileWriteString "*";
#pragma aux FileWriteNewLine "*";
#pragma aux FileFlushBuffer "*";
#pragma aux FileReadString "*";
#pragma aux FileSkipToNextLine "*";
#pragma aux FileWriteChar "*";
#pragma aux FileReadNumber "*";
#pragma aux FileWriteNumber "*";

#pragma aux _cc_local_int0 "*";
#pragma aux _cc_local_int23 "*";

#pragma aux _cc_startup "*";

#endif  /* __WATCOMC__ */

#endif  /* STARTUP_H */
