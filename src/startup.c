/* startup.c -- simple startup library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/stdlib.h"

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

/*** Initialization ***/

bool __near startup_init(void)
{
    _psp = 0;   // FIXME
    return true;
}

void __near startup_done(void)
{
    return;
}

void PUBLIC_CODE custom_shutdown(void);

static void *_oldexit_startup = (void *)0;

bool PUBLIC_CODE custom_startup(void)
{
    if (startup_init())
    {
        _oldexit_startup = exitproc;
        exitproc = custom_shutdown;
        return true;
    }
    else
        return false;
}

void PUBLIC_CODE custom_shutdown(void)
{
    startup_done();
    exitproc = _oldexit_startup;
}
