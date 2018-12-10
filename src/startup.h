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

typedef uint16_t __far _cc_iobuf_proc_t(_cc_iobuf *f);

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

/* Global variables */

extern uint16_t     cc_PrefixSeg;
extern void __far  *cc_ErrorAddr;
extern void *(__far cc_ExitProc);
extern int16_t      cc_ExitCode;
extern int          cc_InOutRes;
extern uint8_t      cc_Test8086;
extern _cc_iobuf    cc_Input;
extern _cc_iobuf    cc_Output;

/* Internal variables */
#define _CC_ATEXIT_MAX 32
extern void *__far _cc_ExitList[_CC_ATEXIT_MAX];
extern uint8_t _cc_ExitCount;

//void _cc_on_exit(void);   // internal

// No return.
void _cc_ExitWithError(int16_t status, void __far *addr);
// No return.
void _cc_Exit(int16_t status);

/* Arguments handling */

extern uint16_t PUBLIC_CODE pascal_paramcount(void);
extern void     PUBLIC_CODE pascal_paramstr(char *dest, uint8_t i);
//extern void __declspec(noreturn) PUBLIC_CODE pascal_Halt(uint16_t status);
extern void     PUBLIC_CODE pascal_Halt(uint16_t status);

uint16_t custom_argc(void);
void     custom_argv(char *dest, uint16_t n, uint8_t i);

/* Application startup */

void _cc_startup(void);

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux cc_PrefixSeg "*";
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

#pragma aux pascal_paramcount "*" modify [    bx cx dx si di es ];
#pragma aux pascal_paramstr   "*" modify [ ax bx cx dx si di es ];
#pragma aux pascal_Halt       "*";

#pragma aux custom_argc "*";
#pragma aux custom_argv "*";

#pragma aux _cc_startup "*";

#endif  /* __WATCOMC__ */

#endif  /* STARTUP_H */
