/* open.c -- part of custom "io" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/string.h"
#include "cc/errno.h"
#include "cc/dos.h"
#include "cc/io.h"
#include "cc/fcntl.h"

#define BADHANDLE -1

/*
 * Input:
 *      `flags' is a file mode bit-field as defined in "cc/fcntl.h" (CC_O_* constants)
 *      `mode' is a file attributes bit-field as defined in "cc/dos.h" (_CC_A_* constants)
 */
int cc_open(const char *filename, int flags, int mode)
{
    int flagsdos;
    int16_t fd;
    uint16_t bytes;
    uint16_t err;
    char xstr[5];

    /*
    DEBUG_print("cc_open(filename=");
    DEBUG_print(filename ? filename : "NULL");
    DEBUG_print(", flags=");
    DEBUG_get_xnum(flags, 4, xstr);
    DEBUG_print(xstr);
    DEBUG_print(", mode=");
    DEBUG_get_xnum(mode, 4, xstr);
    DEBUG_print(xstr);
    DEBUG_print(")" CRLF);
    */

    // Check for unknown bits in flags
    if (flags & !(CC_O_RDONLY | CC_O_WRONLY | CC_O_RDWR | CC_O_CREAT | CC_O_APPEND | CC_O_TRUNC | CC_O_NOINHERIT))
    {
        cc_errno = CC_EINVAL;
        return BADHANDLE;
    }

    // Check access mode in flags
    switch (flags & (CC_O_RDONLY | CC_O_WRONLY | CC_O_RDWR))
    {
    case CC_O_RDONLY:
        if (flags & (CC_O_APPEND | CC_O_TRUNC))
        {
            // We cannot append to or truncate a file in read-only access mode
            cc_errno = CC_EINVAL;
            return BADHANDLE;
        }
        flagsdos = CC_O_RDONLY_DOS;
        break;
    case CC_O_WRONLY:
        flagsdos = CC_O_WRONLY_DOS;
        break;
    case CC_O_RDWR:
        flagsdos = CC_O_RDWR_DOS;
        break;
    default:
        cc_errno = CC_EINVAL;
        return BADHANDLE;
    }

    // Check for unknown bits in mode for `create' option
    // HINT: DOS version
    if ((flags & CC_O_CREAT)
    &&  (mode & !(_CC_A_RDONLY | _CC_A_HIDDEN | _CC_A_SYSTEM | _CC_A_VOLID | _CC_A_SUBDIR | _CC_A_ARCH)))
    {
        cc_errno = CC_EINVAL;
        return BADHANDLE;
    }

    // Check other flags
    switch (flags & (CC_O_APPEND | CC_O_TRUNC))
    {
    case 0:
    case CC_O_APPEND:
    case CC_O_TRUNC:
        break;
    default:
        cc_errno = CC_EINVAL;
        return BADHANDLE;
    }

    if (flags & CC_O_NOINHERIT)
        flagsdos |= CC_O_NOINHERIT_DOS;

    if (!_cc_dos_open(filename, flagsdos, &fd))
    {
        // FIXME: DOS is unable to do atomic append writes. We must do it manually.
        // TODO: We can implement a simple structure to store a file handle and access mode.
        if (flags & CC_O_TRUNC)
            if (_cc_dos_write(fd, NULL, 0, &bytes))
            {
                err = cc_errno;
                _cc_dos_close(fd);
                cc_errno = err;
                return BADHANDLE;
            }
        cc_errno = CC_EZERO;
        return fd;
    }
    else
    {
        if ((cc_errno == CC_ENOENT) && (flags & CC_O_CREAT))
            if (!_cc_dos_creat(filename, mode, &fd))
            {
                // By default DOS opens file in read-write access mode
                if ((flags & (CC_O_RDONLY | CC_O_WRONLY | CC_O_RDWR)) != CC_O_RDWR)
                {
                    // Reopen it in a given access mode
                    _cc_dos_close(fd);
                    if (_cc_dos_open(filename, flagsdos, &fd))
                        return BADHANDLE;
                }
                cc_errno = CC_EZERO;
                return fd;
            }
        return BADHANDLE;
    }
}
