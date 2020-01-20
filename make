#!/bin/bash
## Supported environments:
##   * GNU/Linux
##   * DOS/DJGPP
## Notes:
##   * not all targets are supported under DOS/DJGPP.
##   * all options are passed in double qoutes for DOS/DJGPP compatibility only.
while true; do
    echo 'Select a command to run:'
    select cmdline in \
     exit \
     'make clean' \
     'make all' \
     'make all "DEBUG_COMPILE=1"' \
     'make all "SYSDEBUG=1"' \
     'make all "DEBUG=1"' \
     'make all "DEBUG=1" "SYSDEBUG=1"' \
     'make all "LINKER_TPC=1"' \
     'make all "LINKER_TPC=1" "DEBUG_COMPILE=1"' \
     'make all "LINKER_TPC=1" "SYSDEBUG=1"' \
     'make all "LINKER_TPC=1" "DEBUG=1"' \
     'make all "LINKER_TPC=1" "DEBUG=1" "SYSDEBUG=1"' \
     ; do
        [ x"$cmdline" == x -o x"$REPLY" == x1 ] && exit
        echo $cmdline
        $cmdline
        break
    done
done
