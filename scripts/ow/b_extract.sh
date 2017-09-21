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
# <conio.h>
wlib -d="$DST" "$CLIB" ':inp.c' ':outp.c'
# <i86.h>
wlib -d="$DST" "$CLIB" ':intr.c' ':dointr' ':enable.c' ':disable.c' ':delay.c'
# <env.h>
wlib -d="$DST" "$CLIB" ':getenv.c'
# <dos.h>
wlib -d="$DST" "$CLIB" ':dosret.c' ':d_getvec.c' ':d_setvec.c' ':opendos.c'
wlib -d="$DST" "$CLIB" ':error086.asm' ':gtime086.asm'
# <stdlib.h>
wlib -d="$DST" "$CLIB" ':atoi.c' ':atol.c' ':istable.c' ':strtol.c' ':errno.c'
# <string.h>
wlib -d="$DST" "$CLIB" ':memcmp.c' ':memcpy.c' ':memset.c'
wlib -d="$DST" "$CLIB" ':strchr.c' ':strlen.c' ':strncmp.c' ':strncpy.c' ':strnicmp.c'
