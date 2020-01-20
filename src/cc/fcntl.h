/* fcntl.h -- declarations for custom "fcntl" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_FCNTL_H_INCLUDED
#define _CC_FCNTL_H_INCLUDED

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include <stdint.h>

/* File mode (bit-field) */

#define CC_O_RDONLY     0x00    // open for read only
#define CC_O_WRONLY     0x01    // open for write only
#define CC_O_RDWR       0x02    // open for read and write
#define CC_O_APPEND     0x10    // writes done at end of file
#define CC_O_CREAT      0x20    // create new file
#define CC_O_TRUNC      0x40    // truncate existing file
#define CC_O_NOINHERIT  0x80    // file is not inherited by child process (DOS only)

/*** Aliases ***/

#define O_RDONLY    CC_O_RDONLY
#define O_WRONLY    CC_O_WRONLY
#define O_RDWR      CC_O_RDWR
#define O_APPEND    CC_O_APPEND
#define O_CREAT     CC_O_CREAT
#define O_TRUNC     CC_O_TRUNC
#define O_NOINHERIT CC_O_NOINHERIT

#endif  /* !_CC_FCNTL_H_INCLUDED */
