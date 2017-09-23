/* debug.h -- declarations for debuging.

   This is free and unencumbered software released into the public domain */

#ifndef DEBUG_H
#define DEBUG_H 1

#include "defines.h"

#ifdef __WATCOMC__
#pragma once
#endif

#include "cc/stdio.h"

#define LOG(s) printf(s)

#endif  /* DEBUG_H */
