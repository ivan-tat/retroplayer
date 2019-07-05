#!/bin/bash
# This script must be run in two steps:
# 1) in a GNU/Linux terminal;
# 2) in a DOS/DJGPP Bash shell.

set -e

target="$1"
if [ -z "$target" ]; then
    target=all
fi

case "$target" in
    all)
        ;;
    *)
        echo "Error: unknown target \"$target\"."
        exit 1
        ;;
esac

echo "Building for target \`$target' ..."

PROJDIR="$PWD"
LASTDIR="$PROJDIR"

W_LIB='wlib'

PC='tpc'
PCFLAGS='-gd -m -v -$d+,e-,g+,l+,n+'
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

if [ -z "$DJGPP" ]; then
# !DJGPP
    echo 'Extracting Watcom C specific files...'
    . ./scripts/ow/ow.sh
    . ./scripts/ow/clib.sh
    cd "$PROJDIR/src"
    $W_LIB "$CLIB" ':i4d.asm' ':i4m.asm' ':i8d086.asm'
    mv i4d.o i4d.obj
    mv i4m.o i4m.obj
    mv i8d086.o i8d086.obj
    cd "$PROJDIR"

    if [ ! -f "$PROJDIR/src/cc/cc.lib" ]; then
        tmp=`mktemp`
        find "$PROJDIR/src/cc/" -type f -name '*.obj' -printf '+%p ' | sort -u > $tmp
        wlib -q -t -zld -o "$PROJDIR/src/cc/cc.lib" @$tmp
        rm $tmp
    fi
else
# DJGPP
    cd "$PROJDIR/src"
    $PC $PCFLAGS 'player\plays3m.pas'
    $PC $PCFLAGS 'player\smalls3m.pas'
    $PC $PCFLAGS 'player\playosci.pas'
fi

cd "$PROJDIR"
