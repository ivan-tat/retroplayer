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

#define map_doserrno_to_inoutres(x) (x)

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

static const char NewLine[] = { 13, 10 };

typedef struct _cc_iobuf_reader_t _cc_iobuf_reader;
typedef bool __far _cc_iobuf_reader_proc_t (_cc_iobuf_reader *stream);
typedef struct _cc_iobuf_reader_t
{
    _cc_iobuf *f;
    unsigned start, end, max;       /* ds:si, bx, cx */
    char *dest;                     /* es:di */
    _cc_iobuf_reader_proc_t *next;  /* ax */
};

/* Internal file handling */

void __near cc_AssignFile (_cc_iobuf *f, void *buffer, uint16_t size, char *name);
void __near cc_SetFileBuffer (_cc_iobuf *f, void *buffer, uint16_t size);
void __near cc_SetFileMode (_cc_iobuf *f, uint16_t mode);
void __near cc_SetFileInput (_cc_iobuf *f);
void __near cc_SetFileOutput (_cc_iobuf *f);
void __near cc_SetFileInOut (_cc_iobuf *f);

/* (+|-)InOutRes(+|-): prefix "+" means IO state check on enter, postfix "+" - on exit. */

inoutres_t __far  _sysio_close_0 (_cc_iobuf *f);                        /* -InOutRes+ */
inoutres_t __far  _sysio_close_1 (_cc_iobuf *f);                        /* -InOutRes+ */
inoutres_t __far  _sysio_close (_cc_iobuf *f, bool do_close);           /* -InOutRes+ */
inoutres_t __far  _sysio_call (_cc_iobuf *f, char num);                 /* -InOutRes+ */
inoutres_t __far  _sysio_file_read (_cc_iobuf *f);                      /* -InOutRes- */
inoutres_t __far  _sysio_file_write (_cc_iobuf *f);                     /* -InOutRes- */
inoutres_t __far  _sysio_device_write (_cc_iobuf *f);                   /* -InOutRes- */
inoutres_t __far  _sysio_file_close (_cc_iobuf *f);                     /* -InOutRes- */
bool       __far  _iostream_read_string (_cc_iobuf_reader *stream);     /* +InOutRes+ */
inoutres_t __near _sysio_write_pad_string (_cc_iobuf *f, uint16_t _n);  /* +InOutRes+ */
inoutres_t __near FileWrite (_cc_iobuf *f, void *src, uint16_t _n);     /* +InOutRes+ */
inoutres_t __far  FileSkipToNextLine (_cc_iobuf *f);                    /* +InOutRes+ */
bool       __far  _iostream_find_string_end (_cc_iobuf_reader *stream);
bool       __far  _iostream_end_of_line (_cc_iobuf_reader *stream);
inoutres_t __far  FileWriteNewLine (_cc_iobuf *f);                      /* +InOutRes+ */
inoutres_t __far  FileFlushBuffer (_cc_iobuf *f);                       /* +InOutRes+ */
inoutres_t __far  _sysio_call_1 (_cc_iobuf *f);                         /* +InOutRes+ */
inoutres_t __far  _sysio_call_2 (_cc_iobuf *f);                         /* +InOutRes+ */
char       __far  FileReadChar (_cc_iobuf *f);                          /* +InOutRes+ */
inoutres_t __far  FileWriteChar (_cc_iobuf *f, char _c, uint16_t _n);   /* +InOutRes+ */
char       __far  FileReadString (_cc_iobuf *f, char *dest, uint16_t max);
bool       __far  _iostream_read_line (_cc_iobuf_reader *stream);
inoutres_t __far  FileWriteString (_cc_iobuf *f, char *str, uint16_t _n);/* +InOutRes+ */
int32_t    __far  FileReadNumber (_cc_iobuf *f);                        /* -InOutRes+ */
bool       __far  _iostream_read_find_printable (_cc_iobuf_reader *stream);
bool       __far  _iostream_read_printable (_cc_iobuf_reader *stream);
inoutres_t __far  FileWriteNumber (_cc_iobuf *f, uint32_t value, int n);
inoutres_t __far  _sys_file_open (_cc_iobuf *f);                        /* -InOutRes- */
void       __near _sys_file_append (_cc_iobuf *f);                      /* -InOutRes- */
uint16_t   __near _sys_store_sint32_decimal (int32_t value, char *endptr, char **startptr);
uint16_t   __near _sys_store_uint32 (uint32_t value, uint8_t base, char *endptr, char **startptr);
bool       __near _sys_strtol (char *s, uint16_t len, int32_t *_n, uint16_t *count);
void       __near _sys_clear_dataseg (void);

void __near cc_AssignFile (_cc_iobuf *f, void *buffer, uint16_t size, char *name)
{
    f->handle = cc_UnusedHandle;
    f->mode = cc_fmClosed;
    f->unknown = 0;
    f->pos = 0;
    f->data_size = 0;
    f->buffer = buffer;
    f->proc[0] = _sys_file_open;
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
        /*__attribute__ ((fallthrough));*/  /* GNU */

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

inoutres_t __far _sysio_close_0 (_cc_iobuf *f)
{
    return _sysio_close (f, 0);
}

inoutres_t __far _sysio_close_1 (_cc_iobuf *f)
{
    return _sysio_close (f, 1);
}

inoutres_t __far _sysio_close (_cc_iobuf *f, bool do_close)
{
    inoutres_t status = EINOUTRES_SUCCESS;

    switch (f->mode)
    {
    case cc_fmOutput:
        _sysio_call (f, 1);
        /*__attribute__ ((fallthrough));*/  /* GNU */

    case cc_fmInput:
        if (do_close)
        {
            status = _sysio_call (f, 3);
            f->mode = cc_fmClosed;
        }
        break;

    default:
        status = EINOUTRES_NOT_OPENED;
        cc_InOutRes = status;
        break;
    }

    return status;
}

inoutres_t __far _sysio_call (_cc_iobuf *f, char num)
{
    inoutres_t status;

    status = f->proc[num](f);
    if (status != EINOUTRES_SUCCESS)
        cc_InOutRes = status;

    return status;
}

inoutres_t __far _sysio_file_read (_cc_iobuf *f)
{
    inoutres_t status;
    uint16_t count;

    if (_dos_read (f->handle, f->buffer, f->rec_size, &count) == EZERO)
        status = EINOUTRES_SUCCESS;
    else
    {
        status = map_doserrno_to_inoutres (_doserrno);
        count = 0;
    }

    f->data_size = count;
    f->pos = 0;
    return status;
}

inoutres_t __far _sysio_file_write (_cc_iobuf *f)
{
    inoutres_t status;
    uint16_t pos, count;

    pos = f->pos;
    f->pos = 0;
    if (_dos_write (f->handle, f->buffer, pos, &count) == EZERO)
    {
        if (f->pos != count)
            status = EINOUTRES_WRITE;
        else
            status = EINOUTRES_SUCCESS;
    }
    else
        status = map_doserrno_to_inoutres (_doserrno);

    return status;
}

inoutres_t __far _sysio_device_write (_cc_iobuf *f)
{
    inoutres_t status;
    uint16_t pos, count;

    pos = f->pos;
    f->pos = 0;
    if (_dos_write (f->handle, f->buffer, pos, &count) == EZERO)
        status = EINOUTRES_SUCCESS;
    else
        status = map_doserrno_to_inoutres (_doserrno);

    return status;
}

inoutres_t __far _sysio_file_close (_cc_iobuf *f)
{
    inoutres_t status;

    status = EINOUTRES_SUCCESS;

    if (f->handle >= 5)
        if (_dos_close (f->handle) != EZERO)
            status = map_doserrno_to_inoutres (_doserrno);

    return status;
}

/*
 * IN:
 *      stream: start, next.
 * MODIFIES:
 *      stream: f: pos.
 *      stream: end.
 */
bool __far _iostream_read_string (_cc_iobuf_reader *stream)
{
    _cc_iobuf *f;

    if (cc_InOutRes != EINOUTRES_SUCCESS)
        return false;

    f = stream->f;

    if (f->mode != cc_fmInput)
    {
        cc_InOutRes = EINOUTRES_NOT_INPUT;
        return false;
    }

    do
    {
        if (f->pos != f->data_size)
        {
            stream->end = f->data_size;
            stream->next (stream);
            f->pos = stream->start;
            if (!stream->next)
                break;
        }
        _sysio_call_1 (stream->f);
    } while (f->pos != f->data_size);

    return true;
}

inoutres_t __near _sysio_write_pad_string (_cc_iobuf *f, uint16_t _n)
{
    inoutres_t status;
    uint16_t nf, count;
    /*
     * (nf) bytes free in a buffer
     * (count) bytes to copy in the current iteration
     * (_n) total bytes left to write
     */

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */
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
            do
            {
                nf = f->rec_size - f->pos;
                count = _n;
                if (count >= nf)
                    count = nf;
                memset ((char *) f->buffer + f->pos, ' ', count);
                f->pos += count;
                if (f->pos == f->rec_size)
                    status = _sysio_call_1 (f);
                /* FIXME: no check for status in original code */
                _n -= count;
            } while (_n);
        }
    }

    return status;
}

inoutres_t __near FileWrite (_cc_iobuf *f, void *src, uint16_t _n)
{
    inoutres_t status;
    char *source;
    uint16_t nf, count;
    /*
     * (nf) bytes free in a buffer
     * (count) bytes to copy in the current iteration
     * (_n) total bytes left to write
     */

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */
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
            source = (char *) src;
            do
            {
                nf = f->rec_size - f->pos;
                count = _n;
                if (count >= nf)
                    count = nf;
                memcpy ((char *) f->buffer + f->pos, source, count);
                source += count;
                f->pos += count;
                if (f->pos == f->rec_size)
                    status = _sysio_call_1 (f);
                /* FIXME: no check for status in original code */
                _n -= count;
            } while (_n);
        }
    }

    return status;
}

inoutres_t __far FileSkipToNextLine (_cc_iobuf *f)
{
    _cc_iobuf_reader stream;
    inoutres_t status;

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */
    stream.f = f;
    stream.start = 0;   /* undefined */
    stream.end = 0;     /* undefined */
    stream.max = 0;     /* undefined */
    stream.dest = NULL;
    stream.next = _iostream_find_string_end;

    if (_iostream_read_string (&stream))
        status = cc_InOutRes;
    else
    {
        if (f->proc[2])
            status = _sysio_call_2 (f);
        else
            status = cc_InOutRes;
    }

    return status;
}

bool __far _iostream_find_string_end (_cc_iobuf_reader *stream)
{
    char *buffer, c;

    buffer = (char *)(stream->f->buffer);
    do
    {
        c = buffer [stream->start];
        stream->start++;

        /* 0x0D - CR - carriage return (^M) - '\r' */
        if (c == '\r')
        {
            if (stream->start == stream->end)
            {
                stream->next = _iostream_end_of_line;
                return true;
            }
            else
                return _iostream_end_of_line (stream);
        }

        /* 0x1A - SUB - substitute (^Z) */
        if (c == 0x1a)
        {
            stream->start--;
            stream->next = NULL;
            return true;
        }
    } while (stream->start != stream->end);

    stream->next = _iostream_find_string_end;
    return true;
}

bool __far _iostream_end_of_line (_cc_iobuf_reader *stream)
{
    char *buffer;

    buffer = (char *)(stream->f->buffer);
    /* 0x0A - LF - line feed (^J) - '\n' */
    if (buffer [stream->start] == '\n')
        stream->start++;

    stream->next = NULL;
    return true;
}

inoutres_t __far FileWriteNewLine (_cc_iobuf *f)
{
    inoutres_t status;

    status = FileWrite (f, (void *) NewLine, 2);

    if (status == EINOUTRES_SUCCESS)
        if (f->proc[2])
            status = _sysio_call_2 (f);

    return status;
}

inoutres_t __far FileFlushBuffer (_cc_iobuf *f)
{
    inoutres_t status;

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */
    status = cc_InOutRes;

    if (f->proc[2])
        if (status == EINOUTRES_SUCCESS)
            status = _sysio_call_2 (f);

    return status;
}

inoutres_t __far _sysio_call_1 (_cc_iobuf *f)
{
    inoutres_t status;

    status = f->proc[1](f);
    if (status != EINOUTRES_SUCCESS)
        cc_InOutRes = status;

    return status;
}

inoutres_t __far _sysio_call_2 (_cc_iobuf *f)
{
    inoutres_t status;

    status = f->proc[2](f);
    if (status != EINOUTRES_SUCCESS)
        cc_InOutRes = status;

    return status;
}

char __far FileReadChar (_cc_iobuf *f)
{
    char c = 0x1a;

    if (cc_InOutRes == EINOUTRES_SUCCESS)
    {
        if (f->mode != cc_fmInput)
            cc_InOutRes = EINOUTRES_NOT_INPUT;
        else
        {
            if (f->pos == f->data_size)
            {
                _sysio_call_1 (f);
                if (f->pos != f->data_size)
                    c = 0;
            }
            else
                c = 0;
            if (!c)
            {
                c = *((char *)(f->buffer)+ f->pos);
                f->pos++;
            }
        }
    }

    return c;
}

inoutres_t __far FileWriteChar (_cc_iobuf *f, char _c, uint16_t _n)
{
    inoutres_t status;

    /* NOTE: in some cases return value is an original AX register value which is not implemented here */

    if (_n > 1)
        status = _sysio_write_pad_string (f, _n - 1);
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
            *((char *) (f->buffer) + f->pos) = _c;
            f->pos++;
            if (f->pos == f->rec_size)
                status = _sysio_call_1 (f);
        }
    }

    return status;
}

char __far FileReadString (_cc_iobuf *f, char *dest, uint16_t max)
{
    _cc_iobuf_reader stream;

    stream.f = f;
    stream.start = 0;   /* original SI */
    stream.end = 0;     /* original BX - unused */
    stream.max = max;
    /*stream.dest = dest + 1;*/ /* for Pascal */
    stream.dest = dest;
    stream.next = _iostream_read_line;
    _iostream_read_string (&stream);
    /*dest [0] = stream.end - stream.start - 1;*/   /* for Pascal */
    dest [stream.end] = 0;

    return stream.end;
}

/*
 * IN:
 *      stream: f, buffer, start, end, max, next.
 */
bool __far _iostream_read_line (_cc_iobuf_reader *stream)
{
    char *buffer, c;

    buffer = (char *)(stream->f->buffer);
    do
    {
        c = buffer [stream->start];
        stream->start++;
        /* 0x0D - CR - carriage return (^M) - '\r' */
        /* 0x1A - SUB - substitute (^Z) */
        if ((c == '\r') || (c == 0x1a))
        {
            stream->start--;
            stream->next = NULL;
            return false;
        }
        stream->max--;
    } while ((stream->max) && (stream->start != stream->end));

    if (stream->max)
    {
        stream->next = _iostream_read_line;
        return true;
    }
    else
    {
        stream->next = NULL;
        return false;
    }
}

inoutres_t __far FileWriteString (_cc_iobuf *f, char *str, uint16_t _n)
{
    inoutres_t status;
    uint16_t len;

    len = strlen (str);
    if (_n > len)
        status = _sysio_write_pad_string (f, _n - len);
    else
        status = cc_InOutRes;

    if ((status == EINOUTRES_SUCCESS) && len)
        status = FileWrite (f, str, len);

    return status;
}

int32_t __far FileReadNumber (_cc_iobuf *f)
{
    _cc_iobuf_reader stream;
    char str [32];
    int32_t result;
    uint16_t count;

    stream.f = f;
    stream.start = 0;
    stream.end = 0;     /* unused */
    stream.max = 32;
    stream.dest = str;
    stream.next = _iostream_read_find_printable;

    _iostream_read_string (&stream);

    if (stream.start != stream.end)
    {
        if (_sys_strtol (str, stream.end - stream.start, &result, &count))
        {
            if (!count)
                return result;
        }
        cc_InOutRes = EINOUTRES_NOT_NUMBER;
    }

    return 0;
}

bool __far _iostream_read_find_printable (_cc_iobuf_reader *stream)
{
    char *buffer, c;

    buffer = (char *)(stream->f->buffer);

    do
    {
        c = buffer [stream->start];
        stream->start++;

        if (c > ' ')
        {
            *stream->dest = c;
            stream->dest++;
            stream->max--;
            while ((stream->max) && (stream->start != stream->end))
            {
                c = buffer [stream->start];
                stream->start++;

                if (c <= ' ')
                {
                    /* eot */
                    stream->start--;
                    stream->next = NULL;
                    return false;
                }

                *stream->dest = c;
                stream->dest++;
                stream->max--;
            }

            if (!stream->max)
            {
                stream->next = NULL;
                return false;
            }

            stream->next = _iostream_read_printable;
            return true;
        }

        /* 0x1A - SUB - substitute (^Z) */
        if (c == 0x1a)
        {
            /* eot */
            stream->start--;
            stream->next = NULL;
            return false;
        }

    } while (stream->start != stream->end);

    stream->next = _iostream_read_find_printable;
    return true;
}

bool __far _iostream_read_printable (_cc_iobuf_reader *stream)
{
    char *buffer, c;

    buffer = (char *)(stream->f->buffer);

    do
    {
        c = buffer [stream->start];
        stream->start++;

        if (c <= ' ')
        {
            /* eot */
            stream->start--;
            stream->next = NULL;
            return false;
        }

        *stream->dest = c;
        stream->dest++;
        stream->max--;
    } while ((stream->max) && (stream->start != stream->end));

    if (!stream->max)
    {
        stream->next = NULL;
        return false;
    }

    stream->next = _iostream_read_printable;
    return true;
}

inoutres_t __far FileWriteNumber (_cc_iobuf *f, uint32_t value, int n)
{
    inoutres_t status;
    char tmp[32], *startptr;
    int len;

    len = _sys_store_sint32_decimal (value, &(tmp[32]), &startptr);
    if (len)
        status = _sysio_write_pad_string (f, len);

    /* FIXME: no check for status in original code */

    return FileWrite (f, startptr, n);
}

inoutres_t __far _sys_file_open (_cc_iobuf *f)
{
    uint16_t attr;
    char mode;
    bool create;
    int16_t fd;
    int error;
    cc_ioctl_info_t info;
    void *p1, *p2;

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
            return map_doserrno_to_inoutres (_doserrno);
    }

    if (f->mode == cc_fmInput)
    {
        p1 = _sysio_file_read;
        p2 = NULL;
    }
    else
    {
        info = 0;
        _dos_ioctl_query_flags (f->handle, &info);
        if (info & 0x80)
        {
            p1 = _sysio_device_write;
            p2 = p1;
        }
        else
        {
            if (f->mode == cc_fmInOut)
                _sys_file_append (f);

            p1 = _sysio_file_write;
            p2 = NULL;
        }

        f->mode = cc_fmOutput;
    }

    f->proc[1] = p1;
    f->proc[2] = p2;
    f->proc[3] = _sysio_file_close;
    return EINOUTRES_SUCCESS;
}

void __near _sys_file_append (_cc_iobuf *f)
{
    int32_t newoff;
    uint16_t count, i;
    char *buf;

    _dos_seek (f->handle, 0, SEEK_END_DOS, &newoff);

    if (newoff > f->rec_size)
        newoff -= f->rec_size;
    else
        newoff = 0;

    _dos_seek (f->handle, newoff, SEEK_SET_DOS, &newoff);

    if (_dos_read (f->handle, f->buffer, f->rec_size, &count) != EZERO)
        count = 0;

    buf = (char *) f->buffer;
    for (i = 0; i < count; i++)
        if (buf [i] == 0x1a)
        {
            _dos_seek (f->handle, - count + i, SEEK_END_DOS, &newoff);
            /* truncate file */
            _dos_write (f->handle, NULL, 0, &count);
            return;
        }
}

/* Returns length of converted integer. */
uint16_t __near _sys_store_sint32_decimal (int32_t value, char *endptr, char **startptr)
{
    uint16_t len;

    if (value < 0)
    {
        len = _sys_store_uint32 (-value, 10, endptr, startptr);
        (*startptr)--;
        **startptr = '-';
        return len + 1;
    }
    else
        return _sys_store_uint32 (value, 10, endptr, startptr);
}

/*
 * Returns length of converted integer.
 *
 * IN:
 *      endptr - first byte after the output buffer.
 */
uint16_t __near _sys_store_uint32 (uint32_t value, uint8_t base, char *endptr, char **startptr)
{
    char *s;
    char c;

    s = endptr;
    while (value)
    {
        c = value % base;
        if (c > 9)
            c += 'A' - 10;
        else
            c += '0';
        value /= base;
        s--;
        *s = c;
    }
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

            for (i = 0; i < 4; i++)
            {
                if (value < 0)
                {
                    /* error: numeric overflow */
                    result = false;
                    break;
                }
                value <<= 1;
            }

            if (!result)
                break;  /* exit on numeric overflow */

            value += c;
            s++;
            len--;
        } while (len);

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

            if (value >> 28)
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
    _sys_clear_dataseg ();
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
