#!/bin/bash
# This script must be run in GNU/Linux OR in DOS/DJGPP Bash shell.

RM='rm -f'

cleandir() {
    local DIR="$1"
    echo "Cleaning $DIR ..."
    cd $DIR
    $RM *.err
    $RM *.exe
    $RM *.map
    $RM *.obj
    $RM *.tpu
    cd $OLDPWD
}

while read line; do
    if [ "${line: -1}" == '/' ]; then
        cleandir "$line"
    else
        $RM "$line"
    fi
done < clean.ent

echo 'Done.'
