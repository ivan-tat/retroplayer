/* fopen.c -- part of custom "stdio" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>

#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/string.h"

#include "cc/stdio.h"
#include "cc/stdio/_io.h"

FILE *cc_fopen(const char *path, const char *mode)
{
    uint16_t seg;
    FILE *stream;
    const char *m;
    bool m_read;
    bool m_write;
    bool m_plus;
    bool result;

    if (!_dos_allocmem(_dos_para(sizeof(FILE)), &seg))
    {
        stream = MK_FP(seg, 0);
        pascal_assign(stream, path);

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
        {
            result = pascal_reset(stream);
        }
        else
            if (m_write)
            {
                result = pascal_rewrite(stream);
            }
            else
                result = false;

        if (result)
            return stream;
        else
            _dos_freemem(FP_SEG(stream));
    };

    return NULL;
}
