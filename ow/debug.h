/* debug.h -- declarations for debuging.

   This is free and unencumbered software released into the public domain */

#ifndef DEBUG_H
#define DEBUG_H 1

#ifdef __WATCOMC__
#pragma once
#include "stdio.h"
#endif

#define LOG(s) printf(s)

#endif  /* DEBUG_H */
