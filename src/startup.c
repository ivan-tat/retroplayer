/* startup.c -- simple startup library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "startup/ints.h"
#include "hw/cpu.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/stdlib.h"
#include "cc/string.h"

#include "startup.h"

//extern void __declspec(noreturn) PUBLIC_CODE pascal_Halt(uint16_t status);
extern void PUBLIC_CODE pascal_Halt(uint16_t status);

#ifdef DEFINE_LOCAL_DATA

static uint16_t     cc_PrefixSeg = 0;
static void __far  *cc_ErrorAddr = NULL;
static void (*__far cc_ExitProc) = NULL;
static int16_t      cc_ExitCode = 0;
static uint8_t      cc_Test8086 = 0;
static _cc_iobuf    cc_Input;
static _cc_iobuf    cc_Output;

static void *__far _cc_ExitList[_CC_ATEXIT_MAX] = { 0 };
static uint8_t _cc_ExitCount = 0;

#endif

#define STDINBUF_SIZE 128
#define STDOUTBUF_SIZE 128

static uint8_t _stdin_buf[STDINBUF_SIZE];
static uint8_t _stdout_buf[STDOUTBUF_SIZE];

// Saved critical interrupt vectors
#define SAVEINTVEC_COUNT 19
const static uint8_t SaveIntVecIndexes[SAVEINTVEC_COUNT] =
{
    0x00, 0x02, 0x1B, 0x21, 0x23, 0x24, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x75
};
static void __far *SaveIntVecs[SAVEINTVEC_COUNT];

/* Internal file handling */

void __near cc_AssignFile (_cc_iobuf *f, void *buffer, uint16_t size, char *name);
void __near cc_SetFileBuffer (_cc_iobuf *f, void *buffer, uint16_t size);
void __near cc_SetFileMode (_cc_iobuf *f, uint16_t mode);
void __near cc_SetFileInput (_cc_iobuf *f);
void __near cc_SetFileOutput (_cc_iobuf *f);
void __near cc_SetFileInOut (_cc_iobuf *f);
uint16_t __far _sysio_file_open (_cc_iobuf *f);
void     __far _sysio_close_0 (_cc_iobuf *f);
void     __far _sysio_close_1 (_cc_iobuf *f);
void     __far _sysio_close (_cc_iobuf *f, char mode);
uint16_t __far _sysio_call (_cc_iobuf *f, char num);
void     __far _sysio_file_close (_cc_iobuf *f);
uint16_t __far _sysio_call_1 (_cc_iobuf *f);
uint16_t __far _sysio_call_2 (_cc_iobuf *f);

void __near cc_AssignFile (_cc_iobuf *f, void *buffer, uint16_t size, char *name)
{
    f->handle = cc_UnusedHandle;
    f->mode = cc_fmClosed;
    f->unknown = 0;
    f->pos = 0;
    f->data_size = 0;
    f->buffer = buffer;
    f->proc[0] = _sysio_file_open;
    f->proc[1] = NULL;
    f->proc[2] = NULL;
    f->proc[3] = NULL;
    memset (f->user_data, 0, 26);
    strncpy (f->name, name, cc_PathStr_size);
}

void __near cc_SetFileBuffer (_cc_iobuf *f, void *buffer, uint16_t size)
{
    f->rec_size = size;
    f->buffer = buffer;
    f->pos = 0;
    f->data_size = 0;
}

void __near cc_SetFileMode (_cc_iobuf *f, uint16_t mode)
{
    switch (f->mode)
    {
    case cc_fmInput:
    case cc_fmOutput:
        _sysio_close_1 (f);
        // continuing...

    case cc_fmClosed:
        f->mode = mode;
        f->pos = 0;
        f->data_size = 0;
        if (_sysio_call (f, 0))
            f->mode = cc_fmClosed;
        break;

    default:
        cc_InOutRes = EINOUTRES_NOT_ASSIGNED;
        break;
    }
}

void __near cc_SetFileInput (_cc_iobuf *f)
{
    cc_SetFileMode (f, cc_fmInput);
}

void __near cc_SetFileOutput (_cc_iobuf *f)
{
    cc_SetFileMode (f, cc_fmOutput);
}

void __near cc_SetFileInOut (_cc_iobuf *f)
{
    cc_SetFileMode (f, cc_fmInOut);
}

uint16_t __far _sysio_file_open (_cc_iobuf *f)
{
    // TODO
    return 0;
}

void __far _sysio_close_0 (_cc_iobuf *f)
{
    _sysio_close (f, 0);
}

void __far _sysio_close_1 (_cc_iobuf *f)
{
    _sysio_close (f, 1);
}

void __far _sysio_close (_cc_iobuf *f, char mode)
{
    switch (f->mode)
    {
    case cc_fmOutput:
        _sysio_call (f, 1);
        // continuing...

    case cc_fmInput:
        if (mode)
        {
            _sysio_call (f, 3);
            f->mode = cc_fmClosed;
        }
        break;

    default:
        cc_InOutRes = EINOUTRES_NOT_OPENED;
        break;
    }
}

// Returns zero on success.
uint16_t __far _sysio_call (_cc_iobuf *f, char num)
{
    uint16_t status;

    status = f->proc[num](f);
    if (!status)
        cc_InOutRes = status;

    return status;
}

void __far _sysio_file_close (_cc_iobuf *f)
{
    // TODO
}

// Returns zero on success.
uint16_t __far _sysio_call_1 (_cc_iobuf *f)
{
    uint16_t status;

    status = f->proc[1](f);
    if (!status)
        cc_InOutRes = status;

    return status;
}

// Returns zero on success.
uint16_t __far _sysio_call_2 (_cc_iobuf *f)
{
    uint16_t status;

    status = f->proc[2](f);
    if (!status)
        cc_InOutRes = status;

    return status;
}

/* Error handling */

void __far __stdcall _cc_local_int0(void __far *addr, uint16_t flags)
{
    _cc_ExitWithError(200, addr);
}

void __far __stdcall _cc_local_int23(void __far *addr, uint16_t flags)
{
    _cc_ExitWithError(255, NULL);
}

#define _sys_print_char(c) _cc_dos_console_out(c);

void __near _sys_print_asciz(char *s)
{
    char *p;
    p = s;
    while (*p)
    {
        _sys_print_char(*p);
        p++;
    }
}

void __near _sys_print_hex_digit(uint8_t v)
{
    _sys_print_char(v <= 9 ? v + '0' : v - 10 + 'A');
}

void __near _sys_print_hex_byte(uint8_t v)
{
    _sys_print_hex_digit(v >> 4);
    _sys_print_hex_digit(v & 0xf);
}

void __near _sys_print_hex_word(uint16_t v)
{
    _sys_print_hex_byte(v >> 8);
    _sys_print_hex_byte(v & 0xff);
}

void __near _sys_print_decimal_byte(uint8_t v)
{
    uint8_t a, r;
    a = v / 100;
    r = v % 100;
    _sys_print_char(a + '0');
    a = r / 10;
    r = r % 10;
    _sys_print_char(a + '0');
    _sys_print_char(r + '0');
}

void _cc_on_exit(void)
{
    void (*__far p)();
/*
    while (cc_ExitProc)
    {
        p = cc_ExitProc;
        cc_ExitProc = NULL;
        cc_InOutRes = 0;
        p();
    }
*/
    while (_cc_ExitCount)
    {
        _cc_ExitCount--;
        p = _cc_ExitList[_cc_ExitCount];
        if (p)
        {
            _cc_ExitList[_cc_ExitCount] = NULL;
            p();
        }
    }
}

/* Arguments handling */

uint16_t custom_argc(void)
{
    return pascal_paramcount() + 1;
}

void custom_argv(char *dest, uint16_t n, uint8_t i)
{
    char _s[pascal_String_size];

    pascal_paramstr(_s, i);
    strpastoc(dest, _s, n);
}

/* Application startup */

void _cc_startup(void)
{
    unsigned i;

    cc_PrefixSeg = _cc_dos_getpsp();
    cc_Test8086 = isCPU_8086();
/* Disable for linking with Borland Pascal:
    for (i = 0; i < SAVEINTVEC_COUNT; i++)
        SaveIntVecs[i] = _cc_dos_getvect(SaveIntVecIndexes[i]);

    _cc_dos_setvect(0, _cc_local_int0_asm);
    _cc_dos_setvect(0x23, _cc_local_int23_asm);
    _cc_dos_setvect(0x24, _cc_local_int24_asm);
*/
    cc_AssignFile(&cc_Input, _stdin_buf, STDINBUF_SIZE, "");
    cc_SetFileInput(&cc_Input);
    cc_AssignFile(&cc_Output, _stdout_buf, STDOUTBUF_SIZE, "");
    cc_SetFileOutput(&cc_Output);
}

/* Application shutdown */

void _cc_ExitWithError(int16_t status, void __far *addr)
{
    unsigned i;

    cc_ExitCode = status;

    cc_ErrorAddr = addr != NULL ? MK_FP(FP_SEG(addr) - cc_PrefixSeg - 0x10, FP_OFF(addr)) : addr;

    _cc_on_exit();
/* Disable for linking with Borland Pascal:
    for (i = 0; i < SAVEINTVEC_COUNT; i++)
        _cc_dos_setvect(SaveIntVecIndexes[i], SaveIntVecs[i]);
*/
    _sysio_close_1(&cc_Input);
    _sysio_close_1(&cc_Output);

    if (cc_ErrorAddr)
    {
        _sys_print_asciz("Runtime error ");
        _sys_print_decimal_byte(cc_ExitCode);
        _sys_print_asciz(" at ");
        _sys_print_hex_word(FP_SEG(cc_ErrorAddr));
        _sys_print_char(':');
        _sys_print_hex_word(FP_OFF(cc_ErrorAddr));
        _sys_print_asciz("." CRLF);
    }
/* Disable for linking with Borland Pascal:
    _cc_dos_terminate(cc_ExitCode);
*/
    pascal_Halt(cc_ExitCode);
}

void _cc_Exit(int16_t status)
{
    _cc_ExitWithError(status, NULL);
}
