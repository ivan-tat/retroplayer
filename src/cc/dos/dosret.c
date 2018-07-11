/* dosret.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "cc/errno.h"
#include "cc/dos/dosret.h"

#define MAX_ERR 89
static const signed char _doserr[MAX_ERR] =
{
    CC_EZERO,   /* 0: no error */
    CC_EINVAL,  /* 1: Invalid function number */
    CC_ENOENT,  /* 2: File not found */
    CC_ENOENT,  /* 3: Path not found */
    CC_EMFILE,  /* 4: Too many open files */
    CC_EACCES,  /* 5: Access denied */
    CC_EBADF,   /* 6: Invalid handle */
    CC_ENOMEM,  /* 7: Memory ctrl blocks destroyed */
    CC_ENOMEM,  /* 8: Insufficient memory */
    CC_ENOMEM,  /* 9: Invalid memory block address */
    CC_E2BIG,   /* 10: Invalid environment */
    CC_ENOEXEC, /* 11: Invalid format */
    -1,         /* 12: Invalid access code */
    CC_ERANGE,  /* 13: Invalid data */
    CC_EEXIST,  /* 14: (not used) */
    CC_ENODEV,  /* 15: Invalid drive specified */
    CC_EACCES,  /* 16: Can't remove current dir */
    CC_EXDEV,   /* 17: Not same device */
    CC_ENOENT,  /* 18: No more matching files */
    /*
     * Error codes 19-31 correspond to error codes 0-12 passed in DI to
     * an INT 0x24 critical error handler and they match the values in
     * AL after errors in INT 0x25/0x26.
     */
    CC_EROFS,   /* 19: Write-protected disk */
    -1,         /* 20: Unknown unit ID */
    CC_EAGAIN,  /* 21: Disk drive not ready */
    CC_EINVAL,  /* 22: Unknown command */
    CC_EIO,     /* 23: Disk data error (CRC error) */
    CC_EINVAL,  /* 24: Bad request structure length */
    CC_EIO,     /* 25: Disk seek error */
    CC_EFAULT,  /* 26: Unknown disk media type */
    CC_EIO,     /* 27: Disk sector not found */
    CC_EIO,     /* 28: Printer out of paper */
    CC_EIO,     /* 29: Write fault error */
    CC_EIO,     /* 30: Read fault error */
    CC_EFAULT,  /* 31: General failure */
    CC_EACCES,  /* 32: File sharing violation */
    CC_EACCES,  /* 33: File locking violation */
    CC_EFAULT,  /* 34: Invalid disk change */
    CC_ENFILE,  /* 35: FCB unavailable (too many FCBs) */
    CC_ENOMEM,  /* 36: Sharing buffer overflow */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 37-49: (reserved) */
    CC_EFAULT,  /* 50: Network request not supported */
    -1,         /* 51: Remote computer not listening */
    -1,         /* 52: Duplicate name on network */
    CC_ENXIO,   /* 53: Network name not found */
    CC_EAGAIN,  /* 54: Network busy */
    CC_ENODEV,  /* 55: Network device no longer exists */
    -1,         /* 56: Net BIOS command limit exceeded */
    CC_EIO,     /* 57: Network adapter hardware error */
    -1,         /* 58: Incorrect response from network */
    -1,         /* 59: Unexpected network error */
    -1,         /* 60: Incompatible remote adapter */
    CC_ENOSPC,  /* 61: Print queue full */
    CC_ENOSPC,  /* 62: Not enough space for print file */
    -1,         /* 63: Print file was deleted */
    -1,         /* 64: Network name was deleted */
    CC_EACCES,  /* 65: Access denied */
    -1,         /* 66: Incorrect network device type */
    -1,         /* 67: Network name not found */
    -1,         /* 68: Network name limit exceeded */
    -1,         /* 69: Net BIOS session limit exceeded */
    -1,         /* 70: Temporarily paused */
    -1,         /* 71: Network request not accepted */
    -1,         /* 72: Print or disk redirection is paused */
    -1, -1, -1, -1, -1, -1, -1,  /* 73-79: (reserved) */
    CC_EEXIST,  /* 80: File already exists */
    -1,         /* 81: (reserved) */
    -1,         /* 82: Cannot make directory entry */
    -1,         /* 83: "fail" error from INT 0x24 Critical Error handler */
    -1,         /* 84: Too many redirections */
    -1,         /* 85: Duplicate redirection */
    CC_EACCES,  /* 86: Invalid password */
    CC_EINVAL,  /* 87: Invalid parameter */
    CC_EFAULT   /* 88: Network data fault */
};

unsigned __cc_set_errno_dos(unsigned code)
{
    if (code < MAX_ERR)
    {
        cc_errno = _doserr[code];
        return code;
    }
    else
        return (unsigned)-1;
}
