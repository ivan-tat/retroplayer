/* stdio.c -- custom standard I/O library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#include <i86.h>
#include <stdbool.h>
#include <stdint.h>
#endif

#include "pascal/pascal.h"
#include "ow/dos_.h"
#include "ow/stdio.h"

FILE *fopen(char *path, char *mode)
{
    uint16_t seg;
    FILE *file;
    char *m;
    bool m_read;
    bool m_write;
    bool m_plus;
    bool result;

    if (!_dos_allocmem(sizeof(FILE), &seg))
    {
        file = MK_FP(seg, 0);
        pascal_assign(file, path);

        if (mode)
        {
            m_read = false;
            m_write = false;
            m_plus = false;
            m = mode;
            while (*m)
            {
                switch (*m)
                {
                case 'r':
                    m_read = true;
                    break;
                case 'w':
                    m_write = true;
                    break;
                case '+':
                    m_plus = true;
                    break;
                };
                m++;
            };
        }
        else
        {
            m_read = true;
            m_write = false;
            m_plus = true;
        };

        if (m_read)
            result = pascal_reset(file);
        else
            if (m_write)
                result = pascal_rewrite(file);
            else
                result = false;

        if (result)
            return file;
        else
            _dos_freemem(FP_SEG(file));
    };

    return (void *)0;
}

void fclose(FILE *file)
{
    if (file)
    {
        pascal_close(file);
        _dos_freemem(FP_SEG(file));
    };
}
