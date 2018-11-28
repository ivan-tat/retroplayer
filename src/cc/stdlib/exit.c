/* exit.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "startup.h"

#include "cc/stdlib.h"

void cc_exit(int status)
{
    _cc_Exit(status);
}