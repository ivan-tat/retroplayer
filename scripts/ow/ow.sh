#!/bin/sh
# This script is sourced by other scripts in GNU/Linux environment.
#
# Helper script.

if [ -z "$WATCOM" ]; then
	echo "ERROR: Environment variable WATCOM is not set."
	exit 1
fi
