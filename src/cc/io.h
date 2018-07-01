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

/* Symbolic constants for the lseek() function */

#define CC_SEEK_SET 0   // Seek relative to the start of file
#define CC_SEEK_CUR 1   // Seek relative to current position
#define CC_SEEK_END 2   // Seek relative to the end of the file

/* Aliases */

#define SEEK_SET CC_SEEK_SET
#define SEEK_CUR CC_SEEK_CUR
#define SEEK_END CC_SEEK_END

#endif  /* CC_IO_H */
