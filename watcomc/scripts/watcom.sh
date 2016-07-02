#!/bin/bash
# This script must be run in DOS.
#
# Helper script.

if [ -z "$WATCOM" ]; then
	echo "ERROR: Environment variable WATCOM is not set."
	exit 1
fi
