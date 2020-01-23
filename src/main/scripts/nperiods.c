/* nperiods.c -- static note periods table generator.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BASE_PERIOD 1712
#define HALFTONES 12

int tab [HALFTONES];

int main (void)
{
    int i, n;
    const char *s;

    tab [0] = BASE_PERIOD;
    for (i = 1; i < HALFTONES; i++)
        tab [i] = BASE_PERIOD / pow (2, ((float) i / HALFTONES));

    n = 0;
    for (i = 0; i < HALFTONES; i++)
    {
        if (!i)
            s = "    %4i, ";
        else
            if (n == HALFTONES - 1)
                s = "%4i";
            else
                s = "%4i, ";
        printf (s, tab [n]);
        n++;
    }
    printf ("\n");

    return 0;
}
