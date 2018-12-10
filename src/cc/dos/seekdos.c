/* seekdos.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/io.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"

/*
 * Requirements:
 *      DOS 2.0+
 * Description:
 *      If kind = 0 then move file pointer to start of file + offset.
 *      If kind = 1 then move file pointer to current position + offset.
 *      If kind = 2 then move file pointer to end of file + offset.
 */
/*
unsigned _cc_dos_seek(int fd, long offset, int kind, long *newoffset)
*/
uint16_t __far _cc_dos_seek (int16_t fd, int32_t offset, int16_t kind, int32_t *newoffset)
{
    union CC_REGPACK regs;
    uint16_t error;

    regs.h.ah = 0x42;
    regs.h.al = kind;
    regs.w.bx = fd;
    regs.w.cx = (offset >> 16) & 0xffff;
    regs.w.dx = offset & 0xffff;
    cc_intr(0x21, &regs);
    error = __cc_doserror(&regs);
    if (!error)
        *newoffset = ((int32_t)regs.w.dx << 16) + regs.w.ax;
    return error;
}
