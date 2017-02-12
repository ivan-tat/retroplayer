#!/bin/bash
# This script must be run in DOS.
#
# List contents of a library in human readable form.

. ./djgpp.sh
. ./watcom.sh
. ./clib.sh

DST=tmp
mkdir -p "$DST"
DST="$DST\\${CLIB##*\\}"
DST="${DST%.*}.list"
wlib "$CLIB" | \
grep -E "^([a-zA-Z0-9_]+)\.+([a-zA-Z0-9_.]+)$" | \
sed -r -e "s/^([a-zA-Z0-9_]+)\.+([a-zA-Z0-9_.]+)$/\2 \1/" | \
sort > "$DST"
