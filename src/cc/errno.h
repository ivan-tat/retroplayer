/* errno.h -- declarations for custom "errno" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_ERRNO_H
#define CC_ERRNO_H

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

#include "pascal.h"

extern int16_t cc_errno;

#define CC_EZERO        0   /* No error */
#define CC_ENOENT       1   /* No such file or directory */
#define CC_E2BIG        2   /* Arg list too big */
#define CC_ENOEXEC      3   /* Exec format error */
#define CC_EBADF        4   /* Bad file number */
#define CC_ENOMEM       5   /* Not enough memory */
#define CC_EACCES       6   /* Permission denied */
#define CC_EEXIST       7   /* File exists */
#define CC_EXDEV        8   /* Cross-device link */
#define CC_EINVAL       9   /* Invalid argument */
#define CC_ENFILE       10  /* File table overflow */
#define CC_EMFILE       11  /* Too many open files */
#define CC_ENOSPC       12  /* No space left on device */
/* math errors */
#define CC_EDOM         13  /* Argument too large */
#define CC_ERANGE       14  /* Result too large */
/* file locking error */
#define CC_EDEADLK      15  /* Resource deadlock would occur */
#define CC_EDEADLOCK    15  /* ... */
#define CC_EINTR        16  /* System call interrupted */
#define CC_ECHILD       17  /* Child does not exist */
/* POSIX errors */
#define CC_EAGAIN       18  /* Resource unavailable, try again */
#define CC_EBUSY        19  /* Device or resource busy */
#define CC_EFBIG        20  /* File too large */
#define CC_EIO          21  /* I/O error */
#define CC_EISDIR       22  /* Is a directory */
#define CC_ENOTDIR      23  /* Not a directory */
#define CC_EMLINK       24  /* Too many links */
#define CC_ENOTBLK      25  /* Block device required */
#define CC_ENOTTY       26  /* Not a character device */
#define CC_ENXIO        27  /* No such device or address */
#define CC_EPERM        28  /* Not owner */
#define CC_EPIPE        29  /* Broken pipe */
#define CC_EROFS        30  /* Read-only file system */
#define CC_ESPIPE       31  /* Illegal seek */
#define CC_ESRCH        32  /* No such process */
#define CC_ETXTBSY      33  /* Text file busy */
#define CC_EFAULT       34  /* Bad address */
#define CC_ENAMETOOLONG 35  /* Filename too long */
#define CC_ENODEV       36  /* No such device */
#define CC_ENOLCK       37  /* No locks available in system */
#define CC_ENOSYS       38  /* Unknown system call */
#define CC_ENOTEMPTY    39  /* Directory not empty */
/* additional Standard C error */
#define CC_EILSEQ       40  /* Illegal multibyte sequence */

/*** Aliases ***/

#define errno cc_errno

#define EZERO        CC_EZERO
#define ENOENT       CC_ENOENT
#define E2BIG        CC_E2BIG
#define ENOEXEC      CC_ENOEXEC
#define EBADF        CC_EBADF
#define ENOMEM       CC_ENOMEM
#define EACCES       CC_EACCES
#define EEXIST       CC_EEXIST
#define EXDEV        CC_EXDEV
#define EINVAL       CC_EINVAL
#define ENFILE       CC_ENFILE
#define EMFILE       CC_EMFILE
#define ENOSPC       CC_ENOSPC
/* math errors */
#define EDOM         CC_EDOM
#define ERANGE       CC_ERANGE
/* file locking error */
#define EDEADLK      CC_EDEADLK
#define EDEADLOCK    CC_EDEADLOCK
#define EINTR        CC_EINTR
#define ECHILD       CC_ECHILD
/* POSIX errors */
#define EAGAIN       CC_EAGAIN
#define EBUSY        CC_EBUSY
#define EFBIG        CC_EFBIG
#define EIO          CC_EIO
#define EISDIR       CC_EISDIR
#define ENOTDIR      CC_ENOTDIR
#define EMLINK       CC_EMLINK
#define ENOTBLK      CC_ENOTBLK
#define ENOTTY       CC_ENOTTY
#define ENXIO        CC_ENXIO
#define EPERM        CC_EPERM
#define EPIPE        CC_EPIPE
#define EROFS        CC_EROFS
#define ESPIPE       CC_ESPIPE
#define ESRCH        CC_ESRCH
#define ETXTBSY      CC_ETXTBSY
#define EFAULT       CC_EFAULT
#define ENAMETOOLONG CC_ENAMETOOLONG
#define ENODEV       CC_ENODEV
#define ENOLCK       CC_ENOLCK
#define ENOSYS       CC_ENOSYS
#define ENOTEMPTY    CC_ENOTEMPTY
/* additional Standard C error */
#define EILSEQ       CC_EILSEQ

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux cc_errno "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_ERRNO_H */
