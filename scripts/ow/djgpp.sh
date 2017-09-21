#!/bin/bash
# This script is sourced by other scripts in DOS/DJGPP environment.
#
# Helper script.

if [ -z "$DJGPP" ]; then
	echo 'ERROR: Use DJGPP to run this script.'
	exit 1
fi
