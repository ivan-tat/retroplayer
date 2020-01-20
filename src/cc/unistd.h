/* unistd.h -- declarations for custom "unistd" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#ifndef _CC_UNISTD_H_INCLUDED
#define _CC_UNISTD_H_INCLUDED 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include "pascal.h"

int cc_execv (const char *filename, char *const argv[]);

/*** Aliases ***/

#define execv cc_execv

/*** Linkning ***/

#ifdef __WATCOMC__

#pragma aux cc_execv "*";

#endif  /* __WATCOMC__ */

#endif  /* !_CC_UNISTD_H_INCLUDED */
