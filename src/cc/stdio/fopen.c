/* fopen.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "startup.h"
#include "debugfn.h"
#include "cc/i86.h"
#include "cc/errno.h"
#include "cc/dos.h"
#include "cc/string.h"
#include "cc/stdio.h"
#include "cc/fcntl.h"
#include "cc/io.h"

static const struct
{
    uint16_t openflags;
    uint16_t filemode;
} FILE_MODES[6] =
{
    /* r */
        O_RDONLY,
        //cc_fmInput,   // HINT: not used in Turbo Pascal 7.0
        cc_fmInOut,
    /* w */
        O_WRONLY | O_CREAT | O_TRUNC,
        //cc_fmOutput,  // HINT: not used in Turbo Pascal 7.0
        cc_fmInOut,
    /* a */
        O_WRONLY | O_CREAT | O_APPEND,
        //cc_fmOutput,  // HINT: not used in Turbo Pascal 7.0
        cc_fmInOut,
    /* r+ */
        O_RDWR,
        cc_fmInOut,
    /* w+ */
        O_RDWR | O_CREAT | O_TRUNC,
        cc_fmInOut,
    /* a+ */
        O_RDWR | O_CREAT | O_APPEND,
        cc_fmInOut,
};

int __get_mode(const char *mode)
{
    const char *m;
    char m_read, m_write, m_append, m_binary, m_plus;
    bool result;
    int i;

    m_read = 0;
    m_write = 0;
    m_append = 0;
    m_binary = 0;
    m_plus = 0;
    m = mode;
    result = true;
    while (*m && result)
    {
        switch (*m)
        {
        case 'r':
            if (m_read)
                result = false;
            m_read++;
            break;
        case 'w':
            if (m_write)
                result = false;
            m_write++;
            break;
        case 'a':
            if (m_append)
                result = false;
            m_append++;
            break;
        case 'b':
            if (m_binary)
                result = false;
            m_binary++;
            break;
        case '+':
            if (m_plus)
                result = false;
            m_plus++;
            break;
        default:
            result = false;
            break;
        }
        m++;
    }
    if (result && (m_read + m_write + m_append == 1))
    {
        i = 0;
        if (m_write)    i += 1;
        if (m_append)   i += 2;
        if (m_plus)     i += 3;
        return i;
    }
    else
        return -1;
}

FILE *cc_fopen(const char *path, const char *mode)
{
    uint16_t seg;
    FILE *stream;
    int i;
    int fd;
    int openflags;
    int openmode;

    if ((!path) || (!mode))
    {
        cc_errno = EINVAL;
        cc_InOutRes = EINOUTRES_NOT_ASSIGNED;
        return NULL;
    }

    i = __get_mode(mode);
    if (i < 0)
    {
        cc_errno = CC_EINVAL;
        cc_InOutRes = EINOUTRES_NOT_ASSIGNED;
        return NULL;
    }

    if (_dos_allocmem(_dos_para(sizeof(FILE)), &seg))
    {
        cc_InOutRes = EINOUTRES_NOT_ASSIGNED;
        return NULL;
    }

    stream = MK_FP(seg, 0);

    openflags = FILE_MODES[i].openflags;
    openmode = _CC_A_NORMAL;

    cc_errno = CC_EZERO;
    fd = cc_open(path, openflags, openmode);
    if (fd != -1)
    {
        cc_memset(stream, 0, sizeof(FILE));
        stream->handle = fd;
        stream->mode = FILE_MODES[i].filemode;
        stream->rec_size = 1;
        strncpy(stream->name, path, cc_PathStr_size);
        cc_InOutRes = EINOUTRES_SUCCESS;
        return stream;
    }
    else
    {
        _dos_freemem(FP_SEG(stream));
        cc_InOutRes = EINOUTRES_NOT_OPENED;
        return NULL;
    }
}
