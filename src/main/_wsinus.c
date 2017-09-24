/* _wsinus.c -- static sinus wave table generator.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

int wave[64];

int main(void)
{
    int i, j, n;
    const char *s;

    for (i = 0; i < 64; i++)
        wave[i] = 127 * sin(PI / 32 * i);

    n = 0;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (!j)
                s = "    %4i, ";
            else
                if (n == 63)
                    s = "%4i";
                else
                    s = "%4i, ";
            printf(s, wave[n]);
            n++;
        }
        printf("\n");
    }

    return 0;
}
