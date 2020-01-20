#ifndef _TESTEXEC_H_INCLUDED
#define _TESTEXEC_H_INCLUDED 1

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include "defines.h"

void __noreturn _start_c (void);
int main (int argc, const char **argv);

/*** Linkning ***/

#ifdef __WATCOMC__
#pragma aux _start_c "*";
#pragma aux main "*";
#endif  /* __WATCOMC__ */

#endif  /* !_TESTEXEC_H_INCLUDED */
