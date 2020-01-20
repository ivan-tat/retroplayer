/* filedos.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$filedos$*"
#endif  /* __WATCOMC__ */

#include <stdint.h>
#include "pascal.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"

/*
 * Requirements:
 *      DOS 2.0+
 * Description:
 *      Close file, flush buffers, update directory with correct size, time and date.
 *      Uses internal RAM buffering.
 */
/*
unsigned _cc_dos_close(int fd)
*/
uint16_t __far _cc_dos_close (int16_t fd)
{
    union CC_REGPACK regs;

    regs.h.ah = 0x3e;
    regs.w.bx = fd;
    cc_intr(0x21, &regs);
    return __cc_doserror(&regs);
}

/*
 * Requirements:
 *      DOS 3.3+
 * Description:
 *      Same as "close file" but forces writing operations to disk.
 */
/*
unsigned _cc_dos_commit(int fd)
*/
uint16_t __far _cc_dos_commit (int16_t fd)
{
    union CC_REGPACK regs;

    regs.h.ah = 0x68;
    regs.w.bx = fd;
    cc_intr(0x21, &regs);
    return __cc_doserror(&regs);
}
