/* fexit.c -- part of custom "stdlib" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"

#include "cc/stdlib.h"

extern void __declspec(noreturn) PUBLIC_CODE pascal_halt(uint16_t exitcode);

void cc_exit(int status)
{
    pascal_halt(status);
}
