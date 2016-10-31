#!/bin/bash
# This script must be run in DOS.
#
# Extract all needed files form a library.

. ./djgpp.sh
. ./watcom.sh
. ./clib.sh

DST=tmp
mkdir -p "$DST"
wlib -d="$DST" "$CLIB" '*intr.c' '*dointr'
wlib -d="$DST" "$CLIB" '*memcmp.c' '*memset.c'
wlib -d="$DST" "$CLIB" '*inp.c' '*outp.c'
wlib -d="$DST" "$CLIB" '*i4d' '*i4m' '*i8d086'
