#ifndef TESTCRT_H_INCLUDED
#define TESTCRT_H_INCLUDED 1

#ifdef __WATCOMC__
#pragma once
#endif

#include "defines.h"

void __noreturn _start_c (void);
int main (int argc, const char **argv);

/*** Linkning ***/

#ifdef __WATCOMC__
#pragma aux _start_c "*";
#pragma aux main "*";
#endif

#endif  /* TESTCRT_H_INCLUDED */
