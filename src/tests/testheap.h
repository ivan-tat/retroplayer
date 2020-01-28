#ifndef _TESTHEAP_H_INCLUDED
#define _TESTHEAP_H_INCLUDED 1

#ifdef __WATCOMC__
#pragma once
#endif  /* __WATCOMC__ */

#include "defines.h"

void __noreturn _start_c (void);
int main (void);

/*** Linkning ***/

#ifdef __WATCOMC__
#pragma aux _start_c "*";
#pragma aux main "*";
#endif  /* __WATCOMC__ */

#endif  /* !_TESTHEAP_H_INCLUDED */
