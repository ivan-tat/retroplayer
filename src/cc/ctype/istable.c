/* istable.c -- part of custom character handling library.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org>. */

#include "defines.h"

#ifdef __WATCOMC__
#pragma aux default "$cc$ctype$istable$*"
#endif

#include "cc/ctype.h"

const unsigned char _cc_IsTable[257] =
{
    0,

    /* 0x00 - 0x0f */
    _CC_CNTRL,              /* 0x00  ^@  NUL  null (end string)   */
    _CC_CNTRL,              /* 0x01  ^A  SOH  start of heading    */
    _CC_CNTRL,              /* 0x02  ^B  STX  start of text       */
    _CC_CNTRL,              /* 0x03  ^C  ETX  end of text         */
    _CC_CNTRL,              /* 0x04  ^D  EOT  end of transmission */
    _CC_CNTRL,              /* 0x05  ^E  ENQ  enquiry             */
    _CC_CNTRL,              /* 0x06  ^F  ACK  acknowledge         */
    _CC_CNTRL,              /* 0x07  ^G  BEL  bell                */
    _CC_CNTRL,              /* 0x08  ^H  BS   backspace           */
    _CC_CNTRL | _CC_SPACE,  /* 0x09  ^I  HT   TAB horizontal tab  */
    _CC_CNTRL | _CC_SPACE,  /* 0x0a  ^J  LF   line feed           */
    _CC_CNTRL | _CC_SPACE,  /* 0x0b  ^K  VT   vertical tab        */
    _CC_CNTRL | _CC_SPACE,  /* 0x0c  ^L  FF   form feed           */
    _CC_CNTRL | _CC_SPACE,  /* 0x0d  ^M  CR   carriage return     */
    _CC_CNTRL,              /* 0x0e  ^N  SO   shift out           */
    _CC_CNTRL,              /* 0x0f  ^O  SI   shift in            */

    /* 0x10 - 0x1f */
    _CC_CNTRL,  /* 0x10  ^P  DLE  data line escape     */
    _CC_CNTRL,  /* 0x11  ^Q  DC1  dev ctrl 1 (X-ON)    */
    _CC_CNTRL,  /* 0x12  ^R  DC2  device ctrl 2        */
    _CC_CNTRL,  /* 0x13  ^S  DC3  dev ctrl 3 (X-OFF)   */
    _CC_CNTRL,  /* 0x14  ^T  DC4  device ctrl 4        */
    _CC_CNTRL,  /* 0x15  ^U  NAK  negative acknowledge */
    _CC_CNTRL,  /* 0x16  ^V  SYN  synchronous idle     */
    _CC_CNTRL,  /* 0x17  ^W  ETB  end transmit block   */
    _CC_CNTRL,  /* 0x18  ^X  CAN  cancel               */
    _CC_CNTRL,  /* 0x19  ^Y  EM   end of medium        */
    _CC_CNTRL,  /* 0x1a  ^Z  SUB  substitute           */
    _CC_CNTRL,  /* 0x1b  ^[  ESC  escape               */
    _CC_CNTRL,  /* 0x1c  ^\  FS   file separator       */
    _CC_CNTRL,  /* 0x1d  ^]  GS   group separator      */
    _CC_CNTRL,  /* 0x1e  ^^  RS   record separator     */
    _CC_CNTRL,  /* 0x1f  ^_  US   unit separator       */

    /* 0x20 - 0x2f */
    _CC_PRINT | _CC_SPACE,
    _CC_PRINT | _CC_PUNCT,  /* ! */
    _CC_PRINT | _CC_PUNCT,  /* " */
    _CC_PRINT | _CC_PUNCT,  /* # */
    _CC_PRINT | _CC_PUNCT,  /* $ */
    _CC_PRINT | _CC_PUNCT,  /* % */
    _CC_PRINT | _CC_PUNCT,  /* & */
    _CC_PRINT | _CC_PUNCT,  /* ' */
    _CC_PRINT | _CC_PUNCT,  /* ( */
    _CC_PRINT | _CC_PUNCT,  /* ) */
    _CC_PRINT | _CC_PUNCT,  /* * */
    _CC_PRINT | _CC_PUNCT,  /* + */
    _CC_PRINT | _CC_PUNCT,  /* , */
    _CC_PRINT | _CC_PUNCT,  /* - */
    _CC_PRINT | _CC_PUNCT,  /* . */
    _CC_PRINT | _CC_PUNCT,  /* / */

    /* 0x30 - 0x3f */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 0 */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 1 */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 2 */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 3 */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 4 */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 5 */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 6 */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 7 */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 8 */
    _CC_PRINT | _CC_DIGIT | _CC_XDIGT,   /* 9 */
    _CC_PRINT | _CC_PUNCT,  /* : */
    _CC_PRINT | _CC_PUNCT,  /* ; */
    _CC_PRINT | _CC_PUNCT,  /* < */
    _CC_PRINT | _CC_PUNCT,  /* = */
    _CC_PRINT | _CC_PUNCT,  /* > */
    _CC_PRINT | _CC_PUNCT,  /* ? */

    /* 0x40 - 0x4f */
    _CC_PRINT | _CC_PUNCT,
    _CC_PRINT | _CC_UPPER | _CC_XDIGT,   /* A */
    _CC_PRINT | _CC_UPPER | _CC_XDIGT,   /* B */
    _CC_PRINT | _CC_UPPER | _CC_XDIGT,   /* C */
    _CC_PRINT | _CC_UPPER | _CC_XDIGT,   /* D */
    _CC_PRINT | _CC_UPPER | _CC_XDIGT,   /* E */
    _CC_PRINT | _CC_UPPER | _CC_XDIGT,   /* F */
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,

    /* 0x50 - 0x5f */
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_UPPER,
    _CC_PRINT | _CC_PUNCT,  /* [ */
    _CC_PRINT | _CC_PUNCT,  /* \ */
    _CC_PRINT | _CC_PUNCT,  /* ] */
    _CC_PRINT | _CC_PUNCT,  /* ^ */
    _CC_PRINT | _CC_PUNCT,  /* _ */

    /* 0x60 - 0x6f */
    _CC_PRINT | _CC_PUNCT,
    _CC_PRINT | _CC_LOWER | _CC_XDIGT,   /* a */
    _CC_PRINT | _CC_LOWER | _CC_XDIGT,   /* b */
    _CC_PRINT | _CC_LOWER | _CC_XDIGT,   /* c */
    _CC_PRINT | _CC_LOWER | _CC_XDIGT,   /* d */
    _CC_PRINT | _CC_LOWER | _CC_XDIGT,   /* e */
    _CC_PRINT | _CC_LOWER | _CC_XDIGT,   /* f */
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,

    /* 0x70 - 0x7f */
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_LOWER,
    _CC_PRINT | _CC_PUNCT,  /* { */
    _CC_PRINT | _CC_PUNCT,  /* | */
    _CC_PRINT | _CC_PUNCT,  /* } */
    _CC_PRINT | _CC_PUNCT,  /* ~ */
    _CC_CNTRL,

    /* 0x80 - 0xff */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
