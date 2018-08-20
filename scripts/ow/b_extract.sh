#!/bin/bash
# This script must be run in GNU/Linux.
#
# Extract all needed files form a library.

#. ./djgpp.sh
. ./watcom.sh
. ./clib.sh

DST=tmp
mkdir -p "$DST"
# Watcom specific
wlib -d="$DST" "$CLIB" ':i4d' ':i4m' ':i8d086'
wlib -d="$DST" "$CLIB" ':xmsg.c' ':cstart.c'
# <conio.h>
wlib -d="$DST" "$CLIB" ':inp.c' ':outp.c'
# <i86.h>
wlib -d="$DST" "$CLIB" ':intr.c' ':dointr' ':enable.c' ':disable.c' ':delay.c'
# <env.h>
wlib -d="$DST" "$CLIB" ':getenv.c'
# <dos.h>
wlib -d="$DST" "$CLIB" ':dosret.c' ':d_getvec.c' ':d_setvec.c'
wlib -d="$DST" "$CLIB" ':error086.asm' ':dosexter.c'
wlib -d="$DST" "$CLIB" ':gtime086.asm'
wlib -d="$DST" "$CLIB" ':exv.c' ':exve.c'
wlib -d="$DST" "$CLIB" ':creatdos.c' ':opendos.c' ':filedos.c' ':io086.asm'
# <io.h>
wlib -d="$DST" "$CLIB" ':open.c' ':close.c' ':_clse.c' ':read.c' ':write.c'
# <stdio.h>
wlib -d="$DST" "$CLIB" ':iob.c' ':iobaddr.c'
wlib -d="$DST" "$CLIB" ':fopen.c' ':fclose.c' ':fread.c' ':fwrite.c'
# <stdlib.h>
wlib -d="$DST" "$CLIB" ':atexit.c' ':exit.c' ':_exit.c' ':abort.c'
wlib -d="$DST" "$CLIB" ':atoi.c' ':atol.c' ':istable.c' ':strtol.c' ':errno.c'
wlib -d="$DST" "$CLIB" ':crwdata' ':pspptr.c'
# <string.h>
wlib -d="$DST" "$CLIB" ':memcmp.c' ':memcpy.c' ':memset.c'
wlib -d="$DST" "$CLIB" ':strchr.c' ':strlen.c' ':strncmp.c' ':strncpy.c' ':strnicmp.c'
