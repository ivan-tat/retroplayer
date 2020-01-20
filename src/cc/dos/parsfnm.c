/* parsfnm.c -- part of custom "dos" library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$dos$parsfnm$*"
#endif  /* __WATCOMC__ */

#include "cc/errno.h"
#include "cc/i86.h"
#include "cc/dos.h"
#include "cc/dos/error086.h"

/*
 * Requirements:
 *    DOS 1.0+
 *
 * Description:
 *    Creates an unopened FCB from a text line or command parameter. The
 * text started at "fname" is parsed for the file specifier in the format
 * "D:FILENAME.EXT" and the buffer at "fcb" is filled as a properly-formatted
 * unopened FCB. It converts "*" to a series of "?" and fills default values.
 *    "option" is a bit field:
 *      bit 0: 1=scan off leading separators (path);
 *      bit 1: 1=use FCB drive as a default;
 *      bit 2: 1=use FCB file name as a default;
 *      bit 3: 1=use FCB file extension as a default;
 *      bits 4-7: always 0.
 */

char *_cc_dos_parsfnm (const char *fname, struct cc_dosfcb_t *fcb, char option)
{
    union CC_REGPACK regs;

    regs.w.si = FP_OFF (fname);
    regs.w.ds = FP_SEG (fname);
    regs.w.di = FP_OFF (fcb);
    regs.w.es = FP_SEG (fcb);
    regs.h.al = option;
    regs.h.ah = 0x29;
    cc_intr (0x21, &regs);
    if (regs.h.al = 0xff)
    {
        _cc_doserrno = 15;  /* Invalid drive specified */
        return NULL;
    }
    else
        return MK_FP (regs.w.ds, regs.w.si);
}
