/* creatdos.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"

/*
 * Requirements:
 *      DOS 2.0+
 *      File must not be read-only.
 * Description:
 *      File is opened for read/write in compatibility access mode.
 *      If file exists then it is truncated.
 */
/*
unsigned _cc_dos_creat(const char *fname, unsigned mode, int *fd)
*/
uint16_t PUBLIC_CODE _cc_dos_creat(const char *fname, uint16_t attr, int16_t *fd)
{
    union CC_REGPACK regs;
    uint16_t error;

    regs.h.ah = 0x3c;
    regs.w.cx = attr;
    regs.w.dx = FP_OFF(fname);
    regs.w.ds = FP_SEG(fname);
    cc_intr(0x21, &regs);
    error = __cc_doserror(&regs);
    if (!error)
        *fd = regs.w.ax;
    return error;
}

/*
 * Requirements:
 *      DOS 3.0+
 *      File must not exists.
 * Description:
 *      File is opened for read/write in compatibility access mode.
 */
/*
unsigned _cc_dos_creatnew(const char *fname, unsigned attr, int *fd)
*/
uint16_t PUBLIC_CODE _cc_dos_creatnew(const char *fname, uint16_t attr, int16_t *fd)
{
    union CC_REGPACK regs;
    uint16_t error;

    regs.h.ah = 0x5b;
    regs.w.cx = attr;
    regs.w.dx = FP_OFF(fname);
    regs.w.ds = FP_SEG(fname);
    cc_intr(0x21, &regs);
    error = __cc_doserror(&regs);
    if (!error)
        *fd = regs.w.ax;
    return error;
}
