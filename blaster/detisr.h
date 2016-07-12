#ifndef _DETISR_H
#define _DETISR_H 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "detisr_.h"

void __far __pascal SetDetISRCallback( DetISRCallback_t *p );

#endif /* _DETISR_H */
