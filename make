#!/bin/bash
# This script must be run in two steps:
# 1) in a GNU/Linux terminal;
# 2) in a DOS/DJGPP Bash shell.

set -e

target="$1"
if [ -z "$target" ]; then
    target=all
fi

T_CC=0
T_DOS=0
T_HW=0
T_HW_SB=0
T_MAIN=0
T_PLAYER=0
T_WATCOM=1
T_TP=1

case "$target" in
    all)
        T_CC=1
        T_DOS=1
        T_HW=1
        T_HW_SB=1
        T_MAIN=1
        T_PLAYER=1
        ;;
    cc)
        T_CC=1
        ;;
    dos)
        T_DOS=1
        ;;
    hw)
        T_HW=1
        ;;
    hw_sb)
        T_HW_SB=1
        ;;
    main)
        T_MAIN=1
        ;;
    player)
        T_PLAYER=1
        ;;
    *)
        echo "Error: unknown target \"$target\"."
        exit 1
        ;;
esac

if [ $T_TP == 1 ]; then
    if [ $T_CC == 1 ]; then
        T_CC_TP=1
    fi
    if [ $T_DOS == 1 ]; then
        T_DOS_TP=1
    fi
    if [ $T_HW == 1 ]; then
        T_HW_TP=1
    fi
    if [ $T_HW_SB == 1 ]; then
        T_HW_SB_TP=1
    fi
    if [ $T_MAIN == 1 ]; then
        T_MAIN_TP=1
    fi
    if [ $T_PLAYER == 1 ]; then
        T_PLAYER_TP=1
    fi
    if [ $T_WATCOM == 1 ]; then
        T_WATCOM_TP=1
    fi
else
    T_CC_TP=0
    T_DOS_TP=0
    T_HW_TP=0
    T_HW_SB_TP=0
    T_MAIN_TP=0
    T_PLAYER_TP=0
    T_WATCOM_TP=0
fi

echo "Building for target \`$target' ..."

PROJDIR="$PWD"
LASTDIR="$PROJDIR"

AS="wasm -zq"
CC='wcc'
export WCC="-3 -fp3 -ml -oi -oc -q -r -s -zdp -zff -zgf -zl -zls -zp=1 -zu"
# disable optimization:
# "-oc" - disable <call followed by return> to jump optimization;
# reason: "wdis" incorrectly writes "je near ptr <near_extern_label>"
#   (without "near ptr")
CL='wcl'
DA='wdis'
WLIB='wlib'
PC='tpc -gd -q -v -$d+,e-,g+,l+,n+'

_dir="$PROJDIR/src"
if [ -n "$INCLUDE" ]; then
    export INCLUDE="$INCLUDE;$_dir"
else
    export INCLUDE="$_dir"
fi
unset _dir

ch_dir() {
    local f_locname="$1"
    local f_cd="$2"
    local f_dir="${f_locname%/*}"
    local newdir
    if [ $f_cd == 1 ]; then
        echo "--- File: $f_locname ---"
    else
        echo "--- Skip: $f_locname ---"
    fi
    if [ "$f_dir" == "$f_locname" ]; then
        newdir="$PROJDIR"
    else
        newdir="$PROJDIR/$f_dir"
    fi
    if [ "$LASTDIR" != "$newdir" ]; then
        cd "$newdir"
    fi
}

recompile_obj() {
    local f_obj="$1"
    local f_nam="${f_obj%.obj}"
    local f_tmp="${f_nam}.tmp"
    local f_asm="${f_nam}.asm"
    $DA -a "$f_obj" >"$f_tmp"
    sed -r -e "s/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;\
s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$)/_DATA\1/;" "$f_tmp" >"$f_asm"
    rm -f "$f_tmp"
    $AS -fo="$f_obj" "$f_asm"
}

mkobj_c() {
    local f_c="$1"
    local f_nam="${f_c%.c}"
    local f_obj="${f_nam}.obj"
    local segname
    #segname="${f_nam^^}_TEXT" # such expansion is not supported by Bash-2.05b.0 in DJGPP
    segname="`echo $f_nam | tr a-z A-Z`_TEXT"
    $CC -nt=$segname -fo="$f_obj" "$f_c"
    recompile_obj "$f_obj"
}

mkobj() {
    local f_locname="$1"
    local f_dir="${f_locname%/*}"
    local f_name="${f_locname##*/}"

    case "${f_name##*.}" in
        asm)
            if [ ! -f "$PROJDIR/$f_dir/${f_name%.*}.obj" ]; then
                ch_dir "$f_locname" 1
                $AS -fo="${f_name%.asm}.obj" "$f_name"
            else
                ch_dir "$f_locname" 0
            fi
            ;;
        c)
            if [ ! -f "$PROJDIR/$f_dir/${f_name%.*}.obj" ]; then
                ch_dir "$f_locname" 1
                mkobj_c "$f_name"
            else
                ch_dir "$f_locname" 0
            fi
            ;;
        pas)
            if [ ! -f "$PROJDIR/$f_dir/${f_name%.*}.tpu" ]; then
                ch_dir "$f_locname" 1
                $PC "$f_name"
            else
                ch_dir "$f_locname" 0
            fi
            ;;
    esac
}

mkexe_c() {
    local f_c="$1"
    local f_obj="${f_c%.c}.obj"
    local WCC_OLD="$WCC"
    export WCC="-q -3 -fp3 -fpi87"
    $CL -fo="$f_obj" "$f_c"
    rm -f "$f_obj"
    export WCC="$WCC_OLD"
}

mkexe() {
    local f_locname="$1"
    local f_dir="${f_locname%/*}"
    local f_name="${f_locname##*/}"
    local f_exe="${f_name%.*}.exe"
    local domake

    if [ -f "$PROJDIR/$f_dir/$f_exe" ]; then
        domake=0
    else
        domake=1
    fi

    ch_dir "$f_locname" "$domake"

    if [ $domake == 1 ]; then
        case "${f_name##*.}" in
            c)
                mkexe_c "$f_name"
                ;;
            pas)
                $PC "$f_name"
                ;;
        esac
    fi
}

build_target() {
    local Target=$1
    local BuildType=$2
    local Source="$3"
    if [ $Target -eq 1 ]; then
        case $BuildType in
            exe)
                mkexe "$Source"
                ;;
            obj)
                mkobj "$Source"
                ;;
            *)
                echo "Error: Unknown build type \`$BuildType' for source \`$Source'."
                return 1
                ;;
        esac
    fi
}

if [ -z "$DJGPP" ]; then
if [ $T_WATCOM == 1 ]; then
    echo 'Extracting Watcom C specific files...'
    . ./scripts/ow/ow.sh
    . ./scripts/ow/clib.sh
    cd "$PROJDIR/src/ow"
    $WLIB "$CLIB" ':i4d.asm' ':i4m.asm' ':i8d086.asm'
    mv i4d.o i4d.obj
    mv i4m.o i4m.obj
    mv i8d086.o i8d086.obj
    cd "$PROJDIR"
fi
build_target $T_CC     obj src/cc/i86/delay.c
build_target $T_CC     obj src/cc/i86/disable.c
build_target $T_CC     obj src/cc/i86/dointr.asm
build_target $T_CC     obj src/cc/i86/enable.c
build_target $T_CC     obj src/cc/i86/intr.asm
build_target $T_CC     obj src/cc/conio/inp.c
build_target $T_CC     obj src/cc/conio/outp.c
build_target $T_CC     obj src/cc/string/memcmp.c
build_target $T_CC     obj src/cc/string/memcpy.c
build_target $T_CC     obj src/cc/string/memset.c
build_target $T_CC     obj src/cc/string/strchr.c
build_target $T_CC     obj src/cc/string/strcmp.c
build_target $T_CC     obj src/cc/string/stricmp.c
build_target $T_CC     obj src/cc/string/strlen.c
build_target $T_CC     obj src/cc/string/strncmp.c
build_target $T_CC     obj src/cc/string/strncpy.c
build_target $T_CC     obj src/cc/string/strnicmp.c
build_target $T_WATCOM obj src/ow/dos_.c
build_target $T_WATCOM obj src/ow/malloc.c
build_target $T_WATCOM obj src/ow/stdlib.c
build_target $T_WATCOM obj src/ow/printf.c
build_target $T_WATCOM obj src/ow/stdio.c
build_target $T_DOS    obj src/dos/emstool.c
build_target $T_HW     obj src/hw/cpu.asm
build_target $T_HW     obj src/hw/dma.c
build_target $T_HW     obj src/hw/isr.asm
build_target $T_HW     obj src/hw/pic.c
build_target $T_HW     obj src/hw/sndctl_t.c
build_target $T_HW_SB  obj src/hw/sb/sbio.c
build_target $T_HW_SB  obj src/hw/sb/sbctl.c
build_target $T_HW_SB  obj src/hw/sb/detisr_.asm
build_target $T_HW_SB  obj src/hw/sb/detisr.c
build_target $T_HW_SB  obj src/hw/sb/sndisr_.asm
build_target $T_HW_SB  obj src/hw/sb/sndisr.c
build_target $T_MAIN   obj src/main/mixvars.c
build_target $T_MAIN   obj src/main/s3mvars.c
build_target $T_MAIN   obj src/main/loads3m.c
build_target $T_MAIN   obj src/main/fillvars.c
build_target $T_MAIN   obj src/main/voltab.c
build_target $T_MAIN   obj src/main/posttab.c
build_target $T_MAIN   obj src/main/mixer_.asm
build_target $T_MAIN   obj src/main/mixer.c
build_target $T_MAIN   obj src/main/effects.c
build_target $T_MAIN   obj src/main/readnote.c
build_target $T_MAIN   obj src/main/mixing.c
build_target $T_MAIN   obj src/main/filldma.c
build_target $T_MAIN   obj src/main/s3mplay.c
fi

if [ -n "$DJGPP" ]; then
_dir="$INCLUDE"
if [ -n "$_dir" ]; then
    # replace slashes "/" with backslashes "\" for DOS compatibility
    _dir="${_dir//\//\\}"
    export INCLUDE="$_dir"
fi
unset _dir
build_target $T_WATCOM_TP obj src/ow/watcom.pas
build_target $T_TP        obj src/pascal/pascal.pas
build_target $T_TP        obj src/pascal/strutils.pas
build_target $T_CC_TP     obj src/cc/i86.pas
build_target $T_CC_TP     obj src/cc/conio.pas
build_target $T_CC_TP     obj src/cc/string_.pas
build_target $T_WATCOM_TP obj src/ow/dos_.pas
build_target $T_WATCOM_TP obj src/ow/malloc.pas
build_target $T_WATCOM_TP obj src/ow/stdlib.pas
build_target $T_WATCOM_TP obj src/ow/stdio.pas
build_target $T_DOS_TP    obj src/dos/emstool.pas
build_target $T_HW_TP     obj src/hw/cpu.pas
build_target $T_HW_TP     obj src/hw/dma.pas
build_target $T_HW_TP     obj src/hw/pic.pas
build_target $T_HW_TP     obj src/hw/sndctl_t.pas
build_target $T_HW_SB_TP  obj src/hw/sb/sbio.pas
build_target $T_HW_SB_TP  obj src/hw/sb/sbctl.pas
build_target $T_HW_SB_TP  obj src/hw/sb/detisr.pas
build_target $T_HW_SB_TP  obj src/hw/sb/sndisr.pas
build_target $T_HW_SB_TP  obj src/hw/sb/blaster.pas
build_target $T_MAIN_TP   obj src/main/types.pas
build_target $T_MAIN_TP   obj src/main/mixtypes.pas
build_target $T_MAIN_TP   obj src/main/s3mtypes.pas
build_target $T_MAIN_TP   obj src/main/mixvars.pas
build_target $T_MAIN_TP   obj src/main/s3mvars.pas
build_target $T_MAIN_TP   obj src/main/loads3m.pas
build_target $T_MAIN_TP   obj src/main/fillvars.pas
build_target $T_MAIN_TP   obj src/main/voltab.pas
build_target $T_MAIN_TP   obj src/main/posttab.pas
build_target $T_MAIN_TP   obj src/main/mixer_.pas
build_target $T_MAIN_TP   obj src/main/mixer.pas
build_target $T_MAIN_TP   obj src/main/effvars.pas
build_target $T_MAIN_TP   obj src/main/effects.pas
build_target $T_MAIN_TP   obj src/main/readnote.pas
build_target $T_MAIN_TP   obj src/main/mixing.pas
build_target $T_MAIN_TP   obj src/main/filldma.pas
build_target $T_MAIN_TP   obj src/main/s3mplay.pas
build_target $T_PLAYER_TP exe src/player/plays3m.pas
build_target $T_PLAYER_TP exe src/player/smalls3m.pas
build_target $T_PLAYER_TP obj src/player/lines.asm
build_target $T_PLAYER_TP exe src/player/s3m_osci.pas
fi

cd "$PROJDIR"