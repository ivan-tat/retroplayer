/* unistd.h -- declarations for custom "unistd" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef CC_UNISTD_H
#define CC_UNISTD_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include "pascal.h"

int cc_execv(const char *filename, char *const argv[]);

/* Aliases */

#define execv cc_execv

/* Linkning */

#ifdef __WATCOMC__

#pragma aux cc_execv "*";

#endif  /* __WATCOMC__ */

#endif  /* CC_UNISTD_H */
