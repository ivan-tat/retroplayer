/* startup.h -- declarations for startup.c.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _STARTUP_H_INCLUDED
#define _STARTUP_H_INCLUDED

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

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

#define __IO_OPEN   0
#define __IO_IN_OUT 1
#define __IO_FLUSH  2
#define __IO_CLOSE  3

#pragma pack(push, 1);
typedef struct _cc_iobuf_t {
    uint16_t handle;
    uint16_t mode;
    uint16_t buf_size;
    //char private_data[26];
    uint16_t private;
    uint16_t buf_pos;
    uint16_t buf_end;
    void *buf_ptr;
    union {
        _cc_iobuf_proc_t *by_index[4];
        struct {
            _cc_iobuf_proc_t *open;
            _cc_iobuf_proc_t *in_out;
            _cc_iobuf_proc_t *flush;
            _cc_iobuf_proc_t *close;
        } by_name;
    } io;
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

#define STDINBUF_SIZE 128
#define STDOUTBUF_SIZE 128

extern uint16_t     _cc_psp;
extern uint16_t     _cc_argc;
extern const char **_cc_argv;
extern void __far  *cc_ErrorAddr;
extern void *(__far cc_ExitProc);
extern int16_t      cc_ExitCode;
extern inoutres_t   cc_InOutRes;
extern uint8_t      cc_Test8086;
extern _cc_iobuf    cc_Input;
extern uint8_t      cc_InputBuf[STDINBUF_SIZE];
extern _cc_iobuf    cc_Output;
extern uint8_t      cc_OutputBuf[STDOUTBUF_SIZE];

/* Internal variables */
#define _CC_ATEXIT_MAX 32
extern void *__far _cc_ExitList[_CC_ATEXIT_MAX];
extern uint8_t _cc_ExitCount;

//void _cc_on_exit(void);   // internal

void __noreturn __far __cdecl _cc_ExitWithError (int16_t status, void __far *addr);
void __noreturn _cc_Exit (int16_t status);

/* Arguments handling */

#if LINKER_TPC == 1
extern uint16_t __far __pascal pascal_paramcount(void);
extern void     __far __pascal pascal_paramstr(char *dest, uint8_t i);
extern void     __noreturn __far __pascal pascal_Halt (uint16_t status);
#endif  /* LINKER_TPC == 1 */

/* System unit */

inoutres_t __far cc_IOResult (void);

extern __far _cc_CheckInOutRes (void);

/* (+|-)InOutRes(+|-): prefix "+" means IO state check is done on enter, postfix "+" - on exit. */

/* Buffered file I/O */

void       __far  cc_TextAssign (_cc_iobuf *f, void *buffer, uint16_t size, char *name); /* -InOutRes- */
void       __far  cc_TextSetTextBuf (_cc_iobuf *f, void *buffer, uint16_t size); /* -InOutRes- */
void       __far  cc_TextReset (_cc_iobuf *f);                              /* -InOutRes- */
void       __far  cc_TextRewrite (_cc_iobuf *f);                            /* -InOutRes- */
void       __far  cc_TextAppend (_cc_iobuf *f);                             /* -InOutRes- */
inoutres_t __far  cc_TextFlush (_cc_iobuf *f);                              /* -InOutRes+ */
inoutres_t __far  cc_TextClose (_cc_iobuf *f);                              /* -InOutRes+ */
inoutres_t __far  cc_TextEOL (_cc_iobuf *f);                                /* +InOutRes+ */
unsigned   __far  cc_TextReadString (_cc_iobuf *f, char *dest, uint16_t max); /* +InOutRes? */
char       __far  cc_TextReadChar (_cc_iobuf *f);                           /* +InOutRes+ */
int32_t    __far  cc_TextReadInteger (_cc_iobuf *f);                        /* -InOutRes+ */
inoutres_t __far  cc_TextWriteString (_cc_iobuf *f, char *str, uint16_t padding); /* +InOutRes+ */
inoutres_t __far  cc_TextWriteChar (_cc_iobuf *f, char _c, uint16_t padding); /* +InOutRes+ */
inoutres_t __far  cc_TextWriteInteger (_cc_iobuf *f, uint32_t value, uint16_t padding); /* +InOutRes+ */
inoutres_t __far  cc_TextWriteLn (_cc_iobuf *f);                            /* +InOutRes+ */
inoutres_t __far  cc_TextSync (_cc_iobuf *f);                               /* +InOutRes+ */

/* System */

void __noreturn __far __cdecl _cc_local_int0 (void __far *addr, uint16_t flags);
void __noreturn __far __cdecl _cc_local_int23 (void __far *addr, uint16_t flags);

/* Application startup */

void cc_system_init (void);

/*** Aliases ***/

#define _psp _cc_psp
#define _argc _cc_argc
#define _argv _cc_argv
#define system_init cc_system_init
#define _checkinoutres _cc_CheckInOutRes

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
#pragma aux cc_InputBuf "*";
#pragma aux cc_Output "*";
#pragma aux cc_OutputBuf "*";

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

#pragma aux cc_IOResult "*";
#pragma aux _cc_CheckInOutRes "*";

#pragma aux cc_TextAssign "*";
#pragma aux cc_TextSetTextBuf "*";
#pragma aux cc_TextReset "*";
#pragma aux cc_TextRewrite "*";
#pragma aux cc_TextAppend "*";
#pragma aux cc_TextFlush "*";
#pragma aux cc_TextClose "*";
#pragma aux cc_TextEOL "*";
#pragma aux cc_TextReadString "*";
#pragma aux cc_TextReadChar "*";
#pragma aux cc_TextReadInteger "*";
#pragma aux cc_TextWriteString "*";
#pragma aux cc_TextWriteChar "*";
#pragma aux cc_TextWriteInteger "*";
#pragma aux cc_TextWriteLn "*";
#pragma aux cc_TextSync "*";

#pragma aux _cc_local_int0 "*";
#pragma aux _cc_local_int23 "*";

#pragma aux cc_system_init "*";

#endif  /* __WATCOMC__ */

#endif  /* !_STARTUP_H_INCLUDED */
