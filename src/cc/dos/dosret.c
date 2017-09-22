/* dosret.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include "cc/errno.h"
#include "cc/dos/dosret.h"

#define MAX_ERR 19
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
};

unsigned __cc_set_errno_dos(unsigned code)
{
    if (code <= MAX_ERR)
    {
        cc_errno = _doserr[code];
        return code;
    }
    else
        return (unsigned)-1;
}
