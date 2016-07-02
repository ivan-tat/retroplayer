#!/bin/bash
# This script must be run in DOS.
#
# Extract all needed files form a library.

. ./djgpp.sh
. ./watcom.sh
. ./clib.sh

DST=tmp
mkdir -p "$DST"
wlib -d="$DST" "$CLIB" '*intr.c' '*dointr' '*memcmp.c'
