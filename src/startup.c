/* startup.c -- simple startup library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$startup$*"
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "pascal.h"
#include "startup/ints.h"
#include "hw/cpu.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/stdlib.h"
#include "cc/string.h"
#include "sysdbg.h"
#include "startup.h"

#ifdef DEFINE_LOCAL_DATA

uint16_t     _cc_psp = 0;
uint16_t     _cc_argc = 0;
const char **_cc_argv = NULL;
void __far  *cc_ErrorAddr = NULL;
void (*__far cc_ExitProc) = NULL;
int16_t      cc_ExitCode = 0;
inoutres_t   cc_InOutRes = EINOUTRES_SUCCESS;
uint8_t      cc_Test8086 = 0;
_cc_iobuf    cc_Input;
uint8_t      cc_InputBuf[STDINBUF_SIZE] = { 0 };
_cc_iobuf    cc_Output;
uint8_t      cc_OutputBuf[STDOUTBUF_SIZE] = { 0 };

uint8_t     _cc_ExitCount = 0;
void *__far _cc_ExitList[_CC_ATEXIT_MAX] = { 0 };

#endif

#define map_doserrno_to_inoutres(x) (x)

// Saved critical interrupt vectors
#define SAVEINTVEC_COUNT 19
const static uint8_t SaveIntVecIndexes[SAVEINTVEC_COUNT] =
{
    0x00, 0x02, 0x1B, 0x21, 0x23, 0x24, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x75
};
static void __far *SaveIntVecs[SAVEINTVEC_COUNT] = { 0 };

static const char NewLine[] = { 13, 10 };

typedef struct _cc_iobuf_reader_t _cc_iobuf_reader;
typedef bool __far _cc_iobuf_reader_proc_t (_cc_iobuf_reader *stream);
typedef struct _cc_iobuf_reader_t
{
    _cc_iobuf *f;
    unsigned start, end, max;       /* ds:si, bx, cx */
    char *dest;                     /* es:di */
    unsigned pos;                   /* current destination position */
    _cc_iobuf_reader_proc_t *next;  /* ax */
};

/* Internal file handling */

#if SYSDEBUG == 1
void __near _SYSDEBUG_DUMP_FILE (const char *_file, unsigned _line, const char *_func, _cc_iobuf *f)
{
    _SYSDEBUG_LOG (DBGLOG_INFO, _file, _line, _func, "_SYSDEBUG_DUMP_FILE (%04X:%04X)", FP_SEG (f), FP_OFF (f));
    _SYSDEBUG_LOG (DBGLOG_INFO, NULL, 0, NULL,
        "FILE.handle    = %04X" CRLF
        "FILE.mode      = %04X" CRLF
        "FILE.buf_size  = %04X" CRLF
        "FILE.private   = %04X" CRLF
        "FILE.buf_pos   = %04X" CRLF
        "FILE.buf_end   = %04X" CRLF
        "FILE.buf_ptr   = %04X:%04X",
        "FILE.io.open   = %04X:%04X" CRLF
        "FILE.io.in_out = %04X:%04X" CRLF
        "FILE.io.flush  = %04X:%04X" CRLF
        "FILE.io.close  = %04X:%04X" CRLF
        "FILE.name      = %s",
        f->handle,
        f->mode,
        f->buf_size,
        f->private,
        f->buf_pos,
        f->buf_end,
        FP_SEG (f->buf_ptr), FP_OFF (f->buf_ptr),
        FP_SEG (f->io.by_name.open),   FP_OFF (f->io.by_name.open),
        FP_SEG (f->io.by_name.in_out), FP_OFF (f->io.by_name.in_out),
        FP_SEG (f->io.by_name.flush),  FP_OFF (f->io.by_name.flush),
        FP_SEG (f->io.by_name.close),  FP_OFF (f->io.by_name.close),
        f->name
    );
    SYSDEBUG_dump_mem (&(f->user_data), 16, "FILE.user_data: ");
}

void __near _SYSDEBUG_DUMP_InOutRes (const char *_file, unsigned _line, const char *_func)
{
    _SYSDEBUG_LOG (DBGLOG_INFO, _file, _line, _func, "InOutRes = %i", cc_InOutRes);
}

# define SYSDEBUG_DUMP_FILE(f)    _SYSDEBUG_DUMP_FILE (__FILE__, __LINE__, __func__, f)
# define SYSDEBUG_DUMP_InOutRes() _SYSDEBUG_DUMP_InOutRes (__FILE__, __LINE__, __func__)
#else   /* SYSDEBUG != 1 */
# define SYSDEBUG_DUMP_FILE(f)
# define SYSDEBUG_DUMP_InOutRes()
#endif  /* SYSDEBUG != 1 */

/* (+|-)InOutRes(+|-): prefix "+" means IO state check on enter, postfix "+" - on exit. */

void       __near _cc_TextSetMode (_cc_iobuf *f, uint16_t mode);        /* -InOutRes- */
inoutres_t __near _cc_TextFlushClose (_cc_iobuf *f, bool do_close);     /* -InOutRes+ */
inoutres_t __far  _cc_TextIO (_cc_iobuf *f, char index);                /* -InOutRes+ */
inoutres_t __far  _buffered_read (_cc_iobuf *f);                        /* -InOutRes- */
inoutres_t __far  _buffered_write_file (_cc_iobuf *f);                  /* -InOutRes- */
inoutres_t __far  _buffered_write_device (_cc_iobuf *f);                /* -InOutRes- */
inoutres_t __far  _buffered_close (_cc_iobuf *f);                       /* -InOutRes- */
bool       __far  _stream_read (_cc_iobuf_reader *stream);              /* +InOutRes+ */
inoutres_t __near _buffered_write_pad_string (_cc_iobuf *f, uint16_t _n); /* +InOutRes+ */
inoutres_t __near _cc_TextWrite (_cc_iobuf *f, void *src, uint16_t _n); /* +InOutRes+ */
bool       __far  _stream_find_string_end (_cc_iobuf_reader *stream);
bool       __far  _stream_end_of_line (_cc_iobuf_reader *stream);
inoutres_t __far  _buffered_in_out (_cc_iobuf *f);                      /* +InOutRes+ */
inoutres_t __far  _buffered_flush (_cc_iobuf *f);                       /* +InOutRes+ */
bool       __far  _stream_read_line_start (_cc_iobuf_reader *stream);
bool       __far  _stream_read_integer_start (_cc_iobuf_reader *stream);
bool       __far  _stream_read_integer (_cc_iobuf_reader *stream);
bool       __near _stream_read_integer_char (_cc_iobuf_reader *stream);
inoutres_t __far  _buffered_open (_cc_iobuf *f);                        /* -InOutRes- */
void       __near _buffered_append (_cc_iobuf *f);                      /* -InOutRes- */
uint16_t   __near _sys_store_sint32_decimal (int32_t value, char *endptr, char **startptr);
uint16_t   __near _sys_store_uint32 (uint32_t value, uint8_t base, char *endptr, char **startptr);
bool       __near _sys_strtol (char *s, uint16_t len, int32_t *_n, uint16_t *count);
void       __near _sys_clear_dataseg (void);

inoutres_t __far cc_IOResult (void)
{
    inoutres_t status;
    status = cc_InOutRes;
    cc_InOutRes = EINOUTRES_SUCCESS;
    return status;
}

void __far cc_TextAssign (_cc_iobuf *f, void *buffer, uint16_t size, char *name)
{
    SYSDEBUG_INFO ("Called.");
    f->handle = cc_UnusedHandle;
    f->mode = cc_fmClosed;
    f->buf_size = size;
    f->private = 0;
    f->buf_pos = 0;
    f->buf_end = 0;
    f->buf_ptr = buffer;
    f->io.by_name.open = _buffered_open;
    f->io.by_name.in_out = NULL;
    f->io.by_name.flush = NULL;
    f->io.by_name.close = NULL;
    memset (f->user_data, 0, 26);
    strncpy (f->name, name, cc_PathStr_size);
}

void __far cc_TextSetTextBuf (_cc_iobuf *f, void *buffer, uint16_t size)
{
    SYSDEBUG_INFO_ ("buf=%04x:%04x, size=%u", FP_SEG (buffer), FP_OFF (buffer), size);
    f->buf_size = size;
    f->buf_ptr = buffer;
    f->buf_pos = 0;
    f->buf_end = 0;
    SYSDEBUG_DUMP_FILE (f);
}

void __near _cc_TextSetMode (_cc_iobuf *f, uint16_t mode)
{
    SYSDEBUG_INFO ("Called.");
    switch (f->mode)
    {
    case cc_fmInput:
    case cc_fmOutput:
        cc_TextClose (f);
        __attribute__ ((fallthrough));  /* GNU */

    case cc_fmClosed:
        f->mode = mode;
        f->buf_pos = 0;
        f->buf_end = 0;
        if (_cc_TextIO (f, __IO_OPEN) != EINOUTRES_SUCCESS)
            f->mode = cc_fmClosed;
        break;

    default:
        cc_InOutRes = EINOUTRES_NOT_ASSIGNED;
        break;
    }
}

void __far cc_TextReset (_cc_iobuf *f)
{
    SYSDEBUG_INFO ("Called.");
    _cc_TextSetMode (f, cc_fmInput);
}

void __far cc_TextRewrite (_cc_iobuf *f)
{
    SYSDEBUG_INFO ("Called.");
    _cc_TextSetMode (f, cc_fmOutput);
}

void __far cc_TextAppend (_cc_iobuf *f)
{
    SYSDEBUG_INFO ("Called.");
    _cc_TextSetMode (f, cc_fmInOut);
}

inoutres_t __far cc_TextFlush (_cc_iobuf *f)
{
    inoutres_t status;

    SYSDEBUG_BEGIN ();
    status = _cc_TextFlushClose (f, false);
    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far cc_TextClose (_cc_iobuf *f)
{
    inoutres_t status;

    SYSDEBUG_BEGIN ();
    status = _cc_TextFlushClose (f, true);
    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __near _cc_TextFlushClose (_cc_iobuf *f, bool do_close)
{
    inoutres_t status = EINOUTRES_SUCCESS;

    SYSDEBUG_BEGIN ();
    switch (f->mode)
    {
    case cc_fmOutput:
        _cc_TextIO (f, __IO_IN_OUT);
        __attribute__ ((fallthrough));  /* GNU */

    case cc_fmInput:
        if (do_close)
        {
            status = _cc_TextIO (f, __IO_CLOSE);
            f->mode = cc_fmClosed;
        }
        break;

    default:
        status = EINOUTRES_NOT_OPENED;
        cc_InOutRes = status;
        break;
    }

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far _cc_TextIO (_cc_iobuf *f, char index)
{
    inoutres_t status;

    SYSDEBUG_BEGIN ();
    status = f->io.by_index[index] (f);
    if (status != EINOUTRES_SUCCESS)
        cc_InOutRes = status;

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far _buffered_read (_cc_iobuf *f)
{
    inoutres_t status;
    uint16_t count;

    SYSDEBUG_BEGIN ();
    if (_dos_read (f->handle, f->buf_ptr, f->buf_size, &count) == EZERO)
        status = EINOUTRES_SUCCESS;
    else
    {
        status = map_doserrno_to_inoutres (_doserrno);
        count = 0;
    }

    f->buf_end = count;
    f->buf_pos = 0;
    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far _buffered_write_file (_cc_iobuf *f)
{
    inoutres_t status;
    uint16_t old_pos, numbytes;

    SYSDEBUG_BEGIN ();
    old_pos = f->buf_pos;
    f->buf_pos = 0;
    if (_dos_write (f->handle, f->buf_ptr, old_pos, &numbytes) == EZERO)
    {
        if (old_pos != numbytes)
            status = EINOUTRES_WRITE;
        else
            status = EINOUTRES_SUCCESS;
    }
    else
        status = map_doserrno_to_inoutres (_doserrno);

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far _buffered_write_device (_cc_iobuf *f)
{
    inoutres_t status;
    uint16_t pos, count;

    SYSDEBUG_BEGIN ();
    pos = f->buf_pos;
    f->buf_pos = 0;
    if (_dos_write (f->handle, f->buf_ptr, pos, &count) == EZERO)
        status = EINOUTRES_SUCCESS;
    else
        status = map_doserrno_to_inoutres (_doserrno);

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far _buffered_close (_cc_iobuf *f)
{
    inoutres_t status = EINOUTRES_SUCCESS;

    SYSDEBUG_BEGIN ();
    if (f->handle >= 5)
        if (_dos_close (f->handle) != EZERO)
            status = map_doserrno_to_inoutres (_doserrno);

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

/*
 * IN:
 *      stream: start, next.
 * MODIFIES:
 *      stream: f: buf_pos.
 *      stream: end.
 */
bool __far _stream_read (_cc_iobuf_reader *stream)
{
    _cc_iobuf *f;

    SYSDEBUG_BEGIN ();
    if (cc_InOutRes != EINOUTRES_SUCCESS)
    {
        SYSDEBUG_ERR_ ("Failed (InOutRes=%i)", cc_InOutRes);
        return false;
    }

    f = stream->f;

    if (f->mode != cc_fmInput)
    {
        cc_InOutRes = EINOUTRES_NOT_INPUT;
        SYSDEBUG_ERR_ ("Failed (not %s stream)", "input");
        return false;
    }

    do
    {
        if (f->buf_pos != f->buf_end)
        {
            stream->end = f->buf_end;
            stream->next (stream);
            f->buf_pos = stream->start;
            if (!stream->next)
                break;
        }
        _buffered_in_out (stream->f);
    } while (f->buf_pos != f->buf_end);

    SYSDEBUG_SUCCESS ();
    return true;
}

inoutres_t __near _buffered_write_pad_string (_cc_iobuf *f, uint16_t _n)
{
    inoutres_t status = cc_InOutRes;
    uint16_t nf, count;
    /*
     * (nf) bytes free in a buffer
     * (count) bytes to copy in the current iteration
     * (_n) total bytes left to write
     */

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */
    SYSDEBUG_BEGIN ();

    if (status == EINOUTRES_SUCCESS)
    {
        if (f->mode != cc_fmOutput)
        {
            status = EINOUTRES_NOT_OUTPUT;
            cc_InOutRes = status;
        }
        else
        {
            do
            {
                nf = f->buf_size - f->buf_pos;
                count = _n;
                if (count >= nf)
                    count = nf;
                memset ((char *) f->buf_ptr + f->buf_pos, ' ', count);
                f->buf_pos += count;
                if (f->buf_pos == f->buf_size)
                    status = _buffered_in_out (f);
                /* FIXME: no check for status in original code */
                _n -= count;
            } while (_n);
        }
    }

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __near _cc_TextWrite (_cc_iobuf *f, void *src, uint16_t _n)
{
    inoutres_t status = cc_InOutRes;
    char *source;
    uint16_t nf, count;
    /*
     * (nf) bytes free in a buffer
     * (count) bytes to copy in the current iteration
     * (_n) total bytes left to write
     */

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */
    SYSDEBUG_BEGIN ();

    SYSDEBUG_DUMP_InOutRes ();
    if (status == EINOUTRES_SUCCESS)
    {
        if (f->mode != cc_fmOutput)
        {
            SYSDEBUG_ERR_ ("Failed (not %s stream)", "output");
            status = EINOUTRES_NOT_OUTPUT;
            cc_InOutRes = status;
        }
        else
        {
            source = (char *) src;
            do
            {
                nf = f->buf_size - f->buf_pos;
                SYSDEBUG_INFO_ ("buf_size=%04X, pos=%04X, nf=%04X, _n=%04X.", f->buf_size, f->buf_pos, nf, _n);
                count = _n;
                if (count >= nf)
                    count = nf;
                SYSDEBUG_INFO_ ("%04X:%04X->%04X:%04X+%04X(count=%04X,%04X).",
                    FP_SEG (source), FP_OFF (source), FP_SEG (f->buf_ptr), FP_OFF (f->buf_ptr), f->buf_pos, count, _n
                );
                if (count)
                    SYSDEBUG_dump_mem (source, count, "data: ");
                memcpy ((char *) f->buf_ptr + f->buf_pos, source, count);
                source += count;
                f->buf_pos += count;
                if (f->buf_pos == f->buf_size)
                    status = _buffered_in_out (f);
                /* FIXME: no check for status in original code */
                _n -= count;
            } while (_n);
        }
    }

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far cc_TextEOL (_cc_iobuf *f)
{
    _cc_iobuf_reader stream;
    inoutres_t status;

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */
    SYSDEBUG_BEGIN ();

    stream.f = f;
    stream.start = 0;   /* undefined */
    stream.end = 0;     /* undefined */
    stream.max = 0;     /* undefined */
    stream.dest = NULL;
    stream.pos = 0;     /* undefined */
    stream.next = _stream_find_string_end;

    if (_stream_read (&stream))
        status = cc_InOutRes;
    else
    {
        if (f->io.by_name.flush)
            status = _buffered_flush (f);
        else
            status = cc_InOutRes;
    }

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

bool __far _stream_find_string_end (_cc_iobuf_reader *stream)
{
    char *buffer, c;

    buffer = (char *)(stream->f->buf_ptr);
    do
    {
        c = buffer [stream->start];
        stream->start++;

        /* 0x0D - CR - carriage return (^M) - '\r' */
        if (c == '\r')
        {
            if (stream->start == stream->end)
            {
                stream->next = _stream_end_of_line;
                return true;
            }
            else
                return _stream_end_of_line (stream);
        }

        /* 0x1A - SUB - substitute (^Z) */
        if (c == 0x1a)
        {
            stream->start--;
            stream->next = NULL;
            return true;
        }
    } while (stream->start != stream->end);

    stream->next = _stream_find_string_end;
    return true;
}

bool __far _stream_end_of_line (_cc_iobuf_reader *stream)
{
    char *buffer;

    buffer = (char *)(stream->f->buf_ptr);
    /* 0x0A - LF - line feed (^J) - '\n' */
    if (buffer [stream->start] == '\n')
        stream->start++;

    stream->next = NULL;
    return true;
}

inoutres_t __far cc_TextWriteLn (_cc_iobuf *f)
{
    inoutres_t status;

    SYSDEBUG_BEGIN ();
    status = _cc_TextWrite (f, (void *) NewLine, 2);

    if (status == EINOUTRES_SUCCESS)
        if (f->io.by_name.flush)
            status = _buffered_flush (f);

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far cc_TextSync (_cc_iobuf *f)
{
    inoutres_t status = cc_InOutRes;

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */
    SYSDEBUG_BEGIN ();

    if (f->io.by_name.flush)
        if (status == EINOUTRES_SUCCESS)
            status = _buffered_flush (f);

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far _buffered_in_out (_cc_iobuf *f)
{
    inoutres_t status;

    SYSDEBUG_BEGIN ();

    status = f->io.by_name.in_out (f);
    if (status != EINOUTRES_SUCCESS)
        cc_InOutRes = status;

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

inoutres_t __far _buffered_flush (_cc_iobuf *f)
{
    inoutres_t status;

    SYSDEBUG_BEGIN ();

    status = f->io.by_name.flush (f);
    if (status != EINOUTRES_SUCCESS)
        cc_InOutRes = status;

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

char __far cc_TextReadChar (_cc_iobuf *f)
{
    char c = 0x1a;

    SYSDEBUG_BEGIN ();

    if (cc_InOutRes == EINOUTRES_SUCCESS)
    {
        if (f->mode != cc_fmInput)
            cc_InOutRes = EINOUTRES_NOT_INPUT;
        else
        {
            if (f->buf_pos == f->buf_end)
            {
                _buffered_in_out (f);
                if (f->buf_pos != f->buf_end)
                    c = 0;
            }
            else
                c = 0;
            if (!c)
            {
                c = *((char *)(f->buf_ptr)+ f->buf_pos);
                f->buf_pos++;
            }
        }
    }

    SYSDEBUG_INFO_ ("End (value=%i)", c);
    return c;
}

inoutres_t __far cc_TextWriteChar (_cc_iobuf *f, char _c, uint16_t padding)
{
    inoutres_t status;

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */
    SYSDEBUG_BEGIN ();

    if (padding > 1)
        status = _buffered_write_pad_string (f, padding - 1);
    else
        status = cc_InOutRes;

    if (status == EINOUTRES_SUCCESS)
    {
        if (f->mode != cc_fmOutput)
        {
            status = EINOUTRES_NOT_OUTPUT;
            cc_InOutRes = status;
        }
        else
        {
            *((char *) (f->buf_ptr) + f->buf_pos) = _c;
            f->buf_pos++;
            if (f->buf_pos == f->buf_size)
                status = _buffered_in_out (f);
        }
    }

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

/*
 * Returns length of read string.
 */
unsigned __far cc_TextReadString (_cc_iobuf *f, char *dest, uint16_t max)
{
    _cc_iobuf_reader stream;

    SYSDEBUG_BEGIN ();

    stream.f = f;
    stream.start = 0;   /* original SI */
    stream.end = 0;     /* original BX - unused */
    stream.max = max;
    /*stream.dest = dest + 1;*/ /* for Pascal */
    stream.dest = dest;
    stream.pos = 0;
    stream.next = _stream_read_line_start;
    _stream_read (&stream);
    /*dest [0] = stream.pos;*/  /* for Pascal */
    dest [stream.pos] = 0;

    SYSDEBUG_INFO_ ("End (end=%i, pos=%i)", stream.end, stream.pos);
    return stream.end;
}

/*
 * IN:
 *      stream: f, buffer, start, end, max, next.
 */
bool __far _stream_read_line_start (_cc_iobuf_reader *stream)
{
    char *buffer, c;

    buffer = (char *)(stream->f->buf_ptr);
    do
    {
        c = buffer [stream->start];
        /* 0x0D - CR - carriage return (^M) - '\r' */
        /* 0x1A - SUB - substitute (^Z) */
        if ((c != '\r') && (c != 0x1a))
        {
            stream->start++;
            stream->dest[stream->pos] = c;
            stream->pos++;
            stream->max--;
        }
        else
        {
            stream->next = NULL;
            return false;
        }
    } while ((stream->max) && (stream->start != stream->end));

    stream->next = stream->max ? _stream_read_line_start : NULL;
    return (stream->next != NULL);
}

inoutres_t __far cc_TextWriteString (_cc_iobuf *f, char *str, uint16_t padding)
{
    inoutres_t status;
    uint16_t len;

    SYSDEBUG_BEGIN ();

    len = strlen (str);
    if (padding > len)
        status = _buffered_write_pad_string (f, padding - len);
    else
        status = cc_InOutRes;

    if ((status == EINOUTRES_SUCCESS) && len)
        status = _cc_TextWrite (f, str, len);

    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

#define BUFSIZE 32

int32_t __far cc_TextReadInteger (_cc_iobuf *f)
{
    _cc_iobuf_reader stream;
    char str [BUFSIZE];
    int32_t result;
    uint16_t count;

    SYSDEBUG_BEGIN ();

    stream.f = f;
    stream.start = 0;
    stream.end = 0;     /* unused */
    stream.max = BUFSIZE;
    stream.dest = str;
    stream.pos = 0;
    stream.next = _stream_read_integer_start;

    _stream_read (&stream);

    if (stream.start)
    {
        if (_sys_strtol (str, stream.start, &result, &count))
        {
            if (!count)
            {
                SYSDEBUG_INFO_ ("End (result=%l)", (int32_t) result);
                return result;
            }
        }
        cc_InOutRes = EINOUTRES_NOT_NUMBER;
    }

    SYSDEBUG_INFO_ ("End (result=%l)", (int32_t) 0);
    return 0;
}

#undef BUFSIZE

// TODO: use "ctype.h" library to check symbol type
#define __is_integer_digit(x) ((x) > ' ')

bool __far _stream_read_integer_start (_cc_iobuf_reader *stream)
{
    char *buffer, c;

    buffer = (char *)(stream->f->buf_ptr);

    do
    {
        c = buffer [stream->start];

        if (__is_integer_digit (c))
        {
            /* now read it */
            stream->start++;
            *stream->dest = c;
            stream->dest++;
            stream->max--;

            while ((stream->max) && (stream->start != stream->end))
            {
                if (!_stream_read_integer_char (stream))
                    return false;
            }

            stream->next = stream->max ? _stream_read_integer : NULL;
            return (stream->next != NULL);
        }
        else
        {
            /* 0x1A - SUB - substitute (^Z) */
            if (c == 0x1a)
            {
                /* eot */
                stream->next = NULL;
                return false;
            }
            stream->start++;
        }

    } while (stream->start != stream->end);

    stream->next = _stream_read_integer_start;
    return true;
}

bool __far _stream_read_integer (_cc_iobuf_reader *stream)
{
    do
    {
        if (!_stream_read_integer_char (stream))
            return false;
    } while ((stream->max) && (stream->start != stream->end));

    stream->next = stream->max ? _stream_read_integer : NULL;
    return (stream->next != NULL);
}

bool __near _stream_read_integer_char (_cc_iobuf_reader *stream)
{
    char *buffer, c;

    buffer = (char *)(stream->f->buf_ptr);
    c = buffer [stream->start];

    if (__is_integer_digit (c))
    {
        stream->start++;
        *stream->dest = c;
        stream->dest++;
        stream->max--;
        return true;
    }
    else
    {
        /* eot */
        stream->next = NULL;
        return false;
    }
}

#define BUFSIZE 32

inoutres_t __far cc_TextWriteInteger (_cc_iobuf *f, uint32_t value, uint16_t padding)
{
    inoutres_t status;
    char tmp[BUFSIZE], *startptr;
    uint16_t len;

    SYSDEBUG_BEGIN ();

    len = _sys_store_sint32_decimal (value, &(tmp[BUFSIZE]), &startptr);
    if (padding > len)
        status = _buffered_write_pad_string (f, padding - len);

    /* FIXME: no check for status in original code */

    status = _cc_TextWrite (f, startptr, len);
    SYSDEBUG_INFO_ ("End (status=%i)", status);
    return status;
}

#undef BUFSIZE

inoutres_t __far _buffered_open (_cc_iobuf *f)
{
    uint16_t attr;
    char mode;
    bool create;
    int16_t fd;
    int error;
    cc_ioctl_info_t info;
    void *p_in_out, *p_flush;
    inoutres_t status;

    SYSDEBUG_BEGIN ();

    attr = 0;
    mode = CC_O_RDONLY_DOS;
    create = false;
    if (f->mode != cc_fmInput)
    {
        mode = CC_O_RDWR_DOS;
        f->handle++;
        if (f->mode != cc_fmInOut)
            create = true;
    }

    if (f->name [0])
    {
        if (create)
            error = _dos_creat (f->name, attr, &fd);
        else
            error = _dos_open (f->name, attr, &fd);
        if (error)
        {
            status = map_doserrno_to_inoutres (_doserrno);
            SYSDEBUG_ERR_ ("Failed (doserrno=%i, status=%i)", _doserrno, status);
            return status;
        }
        f->handle = fd;
    }

    if (f->mode == cc_fmInput)
    {
        p_in_out = _buffered_read;
        p_flush  = NULL;
    }
    else
    {
        info = 0;
        _dos_ioctl_query_flags (f->handle, &info);
        if (info & 0x80)
        {
            p_in_out = _buffered_write_device;
            p_flush  = p_in_out;
        }
        else
        {
            if (f->mode == cc_fmInOut)
                _buffered_append (f);

            p_in_out = _buffered_write_file;
            p_flush  = NULL;
        }

        f->mode = cc_fmOutput;
    }

    f->io.by_name.in_out = p_in_out;
    f->io.by_name.flush  = p_flush;
    f->io.by_name.close  = _buffered_close;
    SYSDEBUG_SUCCESS ();
    return EINOUTRES_SUCCESS;
}

void __near _buffered_append (_cc_iobuf *f)
{
    int32_t newoff;
    uint16_t count, i;
    char *buf;

    SYSDEBUG_BEGIN ();

    _dos_seek (f->handle, 0, SEEK_END_DOS, &newoff);

    if (newoff > f->buf_size)
        newoff -= f->buf_size;
    else
        newoff = 0;

    _dos_seek (f->handle, newoff, SEEK_SET_DOS, &newoff);

    if (_dos_read (f->handle, f->buf_ptr, f->buf_size, &count) != EZERO)
        count = 0;

    buf = (char *) f->buf_ptr;
    for (i = 0; i < count; i++)
        if (buf [i] == 0x1a)
        {
            _dos_seek (f->handle, - count + i, SEEK_END_DOS, &newoff);
            /* truncate file */
            _dos_write (f->handle, NULL, 0, &count);
            SYSDEBUG_SUCCESS ();
            return;
        }

    SYSDEBUG_SUCCESS ();
}

/* Returns length of converted integer. */
uint16_t __near _sys_store_sint32_decimal (int32_t value, char *endptr, char **startptr)
{
    uint16_t len;
    bool negate;

    if (value < 0)
    {
        // FIXME: check for INT_MIN and INT_MAX
        negate = true;
        value = -value;
    }
    else
        negate = false;

    len = _sys_store_uint32 (value, 10, endptr, startptr);

    if (negate)
    {
        len++;
        (*startptr)--;
        **startptr = '-';
    }

    return len;
}

/*
 * Returns length of converted integer.
 *
 * IN:
 *      endptr - first byte after the output buffer.
 */
uint16_t __near _sys_store_uint32 (uint32_t value, uint8_t base, char *endptr, char **startptr)
{
    char *s, c;

    s = endptr;
    do
    {
        if (value)
        {
            c = value % base;
            value /= base;
        }
        else
            c = 0;
        if (c > 9)
            c += 'A' - 10;
        else
            c += '0';
        s--;
        *s = c;
    } while (value);
    *startptr = s;
    return endptr - s;
}

/* Part of _sys_strtol(). */
bool __near __check_sign (int32_t *value, bool negate)
{
    bool result;

    if (*value)
    {
        if (negate)
            *value = -*value;

        result = (*value < 0) == (negate != false);
    }
    else
        result = true;

    return result;
}

/*
 * Reads buffer and converts decimal or hexadecimal string into 32-bits signed integer.
 *
 * Format (regexp):
 *   decimal:     [+-]?[0-9]+
 *   hexadecimal: [+-]?\$[0-9A-Fa-f]+
 *
 * Returns true on success and "count" set to number of bytes left at end of input string.
 */
bool __near _sys_strtol (char *s, uint16_t len, int32_t *_n, uint16_t *count)
{
    int32_t value;
    bool negate, result, ok;
    char c, i;

    value = 0;
    negate = false;

    if (!len)
    {
        /* error: unexpected end of input */
        *_n = value;
        *count = len;
        return false;
    }

    if (*s == '+' || *s == '-')
    {
        if (*s == '-')
            negate = true;
        s++;
        len--;
        if (!len)
        {
            /* error: unexpected end of input */
            *_n = value;
            *count = len;
            return false;
        }
    }

    if (*s == '$')
    {
        /* hexadecimal input */
        s++;
        len--;
        if (!len)
        {
            /* error: unexpected end of input */
            *_n = value;
            *count = len;
            return false;
        }

        result = true;
        ok = true;

        do
        {
            c = *s;
            if (c >= '0' && c <= '9')
                c -= '0';
            else
            if (c >= 'A' && c <= 'F')
                c = c - 'A' + 10;
            else
            if (c >= 'a' && c <= 'f')
                c = c - 'a' + 10;
            else
            {
                /* not a hexadecimal digit */
                ok = false; /* skip final sign adjustment - we do it here */
                result = __check_sign (&value, negate);
                break;
            }

            if (value & (0xfUL << 28))
            {
                /* error: numeric overflow */
                result = false;
                break;
            }
            value = value*16 + c;
            s++;
            len--;
        } while (len);

        // FIXME: check for INT_MIN and INT_MAX
        if (ok && result && negate)
            value = -value;
    }
    else
    {
        /* decimal input */
        result = true;

        do
        {
            c = *s;
            if (c >= '0' && c <= '9')
                c -= '0';
            else
                /* not a decimal digit */
                break;

            if (value & (0xfUL << 28))
            {
                /* error: numeric overflow */
                result = false;
                break;
            }
            value = value*10 + c;
            s++;
            len--;
        } while (len);

        if (result)
            result = __check_sign (&value, negate);
    }

    *_n = value;
    *count = len;
    return result;
}

void __near _sys_clear_dataseg (void)
{
    /* TODO: do we really need it? */
}

/* Error handling */

void __noreturn __far __cdecl _cc_local_int0 (void __far *addr, uint16_t flags)
{
    _cc_ExitWithError(200, addr);
}

void __noreturn __far __cdecl _cc_local_int23 (void __far *addr, uint16_t flags)
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

typedef struct _cmdline_parser_data_t {
#if LINKER_TPC != 1
    uint16_t pspseg;
    const char *filename;
    const char *cmdline;
#endif  /* LINKER_TPC != 1 */
    unsigned count;
    unsigned size;
};

#if LINKER_TPC == 1

const char **__near parse_cmdline (bool do_create, struct _cmdline_parser_data_t *_data)
{
    char t[pascal_String_size];
    unsigned args_count, args_size, tab_size, cur_size;
    uint16_t seg;
    const char **tab, **p;
    char *s;
    int i;

    if (!do_create)
    {
        args_count = pascal_paramcount () + 1;
        args_size = 0;
        tab = NULL;
    }
    else
    {
        args_count = _data->count;
        args_size = _data->size;
        tab_size = args_count * sizeof (char *);
        if (_cc_dos_allocmem (_dos_para (tab_size + args_size), &seg) != CC_EZERO)
            return NULL;
        tab = MK_FP (seg, 0);
        p = tab;
        s = (char *) &(tab [args_count]);
        /* "args_count", "args_size" are not used here */
    }

    for (i = 0; i < args_count; i++)
    {
        pascal_paramstr (t, i);
        cur_size = t[0];
        if (!do_create)
            args_size += cur_size + 1;  /* add terminating NULL */
        else
        {
            *p = s;
            p++;
            if (cur_size)
                memcpy (s, t + 1, cur_size);
            s [cur_size] = 0;
            s += cur_size + 1;  /* skip terminating NULL */
        }
    }

    if (!do_create)
    {
        _data->count = args_count;
        _data->size = args_size;
    }

    return tab;
}

#else   /* LINKER_TPC != 1 */

const char __far *_get_filename (const struct cc_dospsp_t __far *psp)
{
    const char __far *s;
    int max;

    s = MK_FP (psp->env_seg, 0);
    if (!s)
        return NULL;

    /* find first empty NULL terminated string of DOS environment */
    max = 32767;    /* scan at most 32K */
    while ((max >= 0) && (*s))
    {
        while ((max >= 0) && (*s))
        {
            max--;
            s++;
        }
        max--;
        s++;
    }

    if (!*s)
    {
        s++;
        /* end of DOS environment */
        if (*((uint16_t __far *)s) == 1)
        {
            s += sizeof (uint16_t);
            /* application's full filename follows (NULL terminated string) */
            return s;
        }
    }

    return NULL;
}

const char **__near parse_cmdline (bool do_create, struct _cmdline_parser_data_t *_data)
{
    const struct cc_dospsp_t __far *psp;
    const char __far *filename;
    const char __far *cl;
    unsigned args_count, args_size, tab_size, len, cur_spaces, cur_size;
    uint16_t seg;
    const char **tab, **p;
    char *s;

    if (!do_create)
    {
        if (_data->pspseg)
        {
            psp = MK_FP (_data->pspseg, 0);
            filename = _get_filename (psp);
            cl = &(psp->param_str);
        }
        else
        {
            filename = NULL;
            cl = NULL;
        }
        _data->filename = filename;
        _data->cmdline = cl;
        args_count = 1; /* the first argument is always a filename */
        if (filename)
            args_size = 0;  /* we don't have to write a filename */
        else
            args_size = 1;  /* we have to write an empty NULL terminated string */
        tab = NULL;
    }
    else
    {
        filename = _data->filename;
        cl = _data->cmdline;
        args_count = _data->count;
        args_size = _data->size;
        tab_size = args_count * sizeof (char *);
        if (_cc_dos_allocmem (_dos_para (tab_size + args_size), &seg) != CC_EZERO)
            return NULL;
        tab = MK_FP (seg, 0);
        p = tab;
        s = (char *) &(tab [args_count]);
        if (filename)
            *p = filename;
        else
        {
            /* we have to save an empty NULL terminated string for filename */
            s[0] = 0;
            s++;
            *p = s;
        }
        p++;
        /* "args_count", "args_size" are not used here */
    }

    if (cl)
    {
        len = cl[0];
        if (len)
        {
            if (len > 127)
                len = 127;
            cl++;
            cur_spaces = 0;
            cur_size = 0;
            do
            {
                if ((*cl == 0) || (*cl == 9) || (*cl == 32))
                {
                    if (cur_size)
                    {
                        /* got argument */
                        if (!do_create)
                        {
                            args_count++;
                            args_size += cur_size + 1;  /* add terminating NULL */
                        }
                        else
                        {
                            *p = s;
                            p++;
                            memcpy (s, cl - cur_size, cur_size);
                            s [cur_size] = 0;
                            s += cur_size + 1;  /* skip terminating NULL */
                        }
                        cur_size = 0;
                        cur_spaces = 0;
                    }
                    cur_spaces++;
                }
                else
                    cur_size++;
                cl++;
                len--;
            } while (len);

            if (cur_size)
            {
                /* got argument */
                if (!do_create)
                {
                    args_count++;
                    args_size += cur_size + 1;  /* add terminating NULL */
                }
                else
                {
                    *p = s;
                    //~ p++;
                    memcpy (s, cl - cur_size, cur_size);
                    s [cur_size] = 0;
                    //~ s += cur_size + 1;  /* skip terminating NULL */
                }
            }
        }
    }

    if (!do_create)
    {
        _data->count = args_count;
        _data->size = args_size;
    }

    return tab;
}

#endif  /* LINKER_TPC != 1 */

/* Application startup */

void cc_system_init (void)
{
    unsigned i;
    struct _cmdline_parser_data_t data;

#if LINKER_TPC == 1
    _cc_psp = _cc_dos_getpsp();
#endif  /* LINKER_TPC == 1 */
    _sys_clear_dataseg ();
    cc_Test8086 = isCPU_8086();
#if LINKER_TPC == 1
#else /* LINKER_TPC != 1 */
    for (i = 0; i < SAVEINTVEC_COUNT; i++)
        SaveIntVecs[i] = _cc_dos_getvect(SaveIntVecIndexes[i]);

    _cc_dos_setvect(0, _cc_local_int0_asm);
    _cc_dos_setvect(0x23, _cc_local_int23_asm);
    _cc_dos_setvect(0x24, _cc_local_int24_asm);
    data.pspseg = _cc_psp;
#endif  /* LINKER_TPC != 1 */
    parse_cmdline (false, &data);
    _cc_argv = parse_cmdline (true, &data);
    if (_cc_argv)
        _cc_argc = data.count;
    else
        _cc_argc = 0;
    cc_TextAssign (&cc_Input, cc_InputBuf, STDINBUF_SIZE, "");
    cc_TextReset (&cc_Input);
    cc_TextAssign (&cc_Output, cc_OutputBuf, STDOUTBUF_SIZE, "");
    cc_TextRewrite (&cc_Output);
}

/* Application shutdown */

void __noreturn __far __cdecl _cc_ExitWithError (int16_t status, void __far *addr)
{
    unsigned i;

    cc_ExitCode = status;

    cc_ErrorAddr = addr != NULL ? MK_FP (FP_SEG (addr) - _cc_psp - 0x10, FP_OFF (addr)) : addr;

    _cc_on_exit();

#if LINKER_TPC != 1
    for (i = 0; i < SAVEINTVEC_COUNT; i++)
        _cc_dos_setvect(SaveIntVecIndexes[i], SaveIntVecs[i]);
#endif  /* LINKER_TPC != 1 */
    if (_cc_argv)
        _cc_dos_freemem (FP_SEG (_cc_argv));

    cc_TextClose (&cc_Input);
    cc_TextClose (&cc_Output);

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
#if LINKER_TPC != 1
    _cc_dos_terminate(cc_ExitCode);
#else
    pascal_Halt(cc_ExitCode);
#endif  /* LINKER_TPC != 1 */
}

void __noreturn _cc_Exit (int16_t status)
{
    _cc_ExitWithError(status, NULL);
}
