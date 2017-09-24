/* _wsquare.c -- static square wave table generator.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int wave[64];

int main(void)
{
    int i, j, n;
    const char *s;

    for (i = 0; i < 64; i++)
        wave[i] = 127 - 255 * (i / 32);

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
