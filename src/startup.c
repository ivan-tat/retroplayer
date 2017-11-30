/* startup.c -- simple startup library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdint.h>

#include "pascal.h"

#include "startup.h"

uint16_t PUBLIC_CODE custom_argc(void)
{
    return pascal_paramcount() + 1;
}

void PUBLIC_CODE custom_argv(char *dest, uint16_t n, uint8_t i)
{
    char _s[pascal_String_size];

    pascal_paramstr(_s, i);
    strpastoc(dest, _s, n);
}
