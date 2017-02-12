#!/bin/bash
# This script must be run in DOS.
#
# Helper script.

CLIB="$WATCOM"
if [ "${CLIB:-1}" = '\' ]; then CLIB="${CLIB:1,-1}"; fi
CLIB="$CLIB\\lib286\\dos\\clibl.lib"
if [ ! -f "$CLIB" ]; then
	echo "ERROR: Library $CLIB not found."
	exit 1
fi
