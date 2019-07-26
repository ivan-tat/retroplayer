/* io.h -- declarations for custom "io" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_IO_H
#define CC_IO_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>
#include "pascal.h"

/* Symbolic constants for the cc_lseek() function */

#define CC_SEEK_SET 0   // Seek relative to the start of file
#define CC_SEEK_CUR 1   // Seek relative to current position
#define CC_SEEK_END 2   // Seek relative to the end of the file

int cc_open(const char *filename, int flags, int mode);
int cc_close(int fd);
/*
off_t   cc_lseek(int fd, off_t offset, int whence);
ssize_t cc_read(int fd, void *buf, size_t count);
ssize_t cc_write(int fd, const void *buf, size_t count);
*/
int32_t cc_lseek(int fd, int32_t offset, int whence);
int32_t cc_read(int fd, void *buf, size_t count);
int32_t cc_write(int fd, void *buf, size_t count);

/*** Aliases ***/

#define SEEK_SET CC_SEEK_SET
#define SEEK_CUR CC_SEEK_CUR
#define SEEK_END CC_SEEK_END

#define open cc_open
#define close cc_close

#define lseek cc_lseek
#define read cc_read
#define write cc_write

/*** Linking ***/

#ifdef __WATCOMC__

#pragma aux cc_open "*";
#pragma aux cc_close "*";

#pragma aux cc_lseek "*";
#pragma aux cc_read "*";
#pragma aux cc_write "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_IO_H */
