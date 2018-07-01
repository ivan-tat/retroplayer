/* fcntl.h -- declarations for custom "fcntl" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_FCNTL_H
#define CC_FCNTL_H

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include <stdint.h>

/* File mode (bit-field) */

#define CC_O_RDONLY     0       // open for read only
#define CC_O_WRONLY     1       // open for write only
#define CC_O_RDWR       2       // open for read and write
#define CC_O_NOINHERIT  0x80    // file is not inherited by child process (DOS only)

/* Aliases */

#define O_RDONLY    CC_O_RDONLY
#define O_WRONLY    CC_O_WRONLY
#define O_RDWR      CC_O_RDWR
#define O_NOINHERIT CC_O_NOINHERIT

#endif  /* CC_FCNTL_H */
