#!/bin/bash
# This script must be run in two steps:
# 1) in a GNU/Linux terminal;
# 2) in a DOS/DJGPP Bash shell.

set -e
shopt -s extglob

# $1=message
error_exit() {
    echo "Error: $1" >&2
    exit 1
}

if [[ $# -eq 0 ]]; then
    cat <<EOT
Usage:
    $0 [ options ] [ target ] [ options ]
Where:
    option: in the form "name=value"
    target: "all"
EOT
    if [[ -n "$DJGPP" ]]; then
        cat <<EOT

Note: You are running in a DJGPP DOS environment. Use "" to surround options.
EOT
    fi
    exit
fi

target=''

while [[ $# -ne 0 ]]; do
    arg="$1"
    if [[ "$arg" =~ .*=.* ]]; then
        eval export "$arg"
        shift 1
    else
        if [[ -n "$target" ]]; then
            error_exit "Multiple targets are not supported."
        fi
        target="$1"
        shift 1
    fi
done

case "$target" in
    all)
        ;;
    *)
        error_exit "Unknown target '$target'."
        ;;
esac

echo "Building for target \`$target' ..."
if [[ $DEBUG -eq 1 ]]; then
    echo "Note: DEBUG is on."
fi

PROJDIR="$PWD"
SRCDIR="$PROJDIR/src"

WLIB='wlib'
WLIBFLAGS='-q'

PC='tpc'
PCDEBUG='-gd -v -$d+,l+'
if [[ $DEBUG -eq 1 ]]; then
    PCDEBUG="$PCDEBUG"' -dDEBUG'
fi
PCFLAGS="$PCDEBUG"' -m -$e-,g+,n+'

# Description of options for "wlib":
# -q    don't print header

# Description of options for "tpc":
# -gd   detailed map file
# -m    make modified units
# -q    quiet compile
# -v    debug information in EXE
# -$d+  debug information (yes)
# -$e-  80x87 emulation (no)
# -$g+  80286 instructions (yes)
# -$l+  local debug symbols (yes)
# -$n+  80x87 instructions (yes)

if [[ -z "$DJGPP" ]]; then
# !DJGPP
    echo 'Extracting Watcom C specific files...'
    . ./scripts/ow/ow.sh
    . ./scripts/ow/clib.sh
    cd "$SRCDIR"
    $WLIB $WLIBFLAGS "$CLIB" ':i4d.asm' ':i4m.asm' ':i8d086.asm'
    mv i4d.o i4d.obj
    mv i4m.o i4m.obj
    mv i8d086.o i8d086.obj
else
# DJGPP
    cd "$SRCDIR"
    $PC $PCFLAGS 'player\plays3m.pas'
    $PC $PCFLAGS 'player\smalls3m.pas'
    $PC $PCFLAGS 'player\playosci.pas'
fi

cd "$PROJDIR"
echo 'Done.'
