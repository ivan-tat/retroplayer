#!/bin/bash
# This script must be run in two steps:
# 1) in a GNU/Linux terminal;
# 2) in a DOS/DJGPP Bash shell.

set -e

target="$1"
if [ -z "$target" ]; then
    target=all
fi

T_DOS=0
T_HW=0
T_HW_SB=0
T_MAIN=0
T_PLAYER=0
T_WATCOM=1
T_TP=1

case "$target" in
    all)
        T_DOS=1
        T_HW=1
        T_HW_SB=1
        T_MAIN=1
        T_PLAYER=1
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
export WCC="-3 -fp3 -ml -oi -oc -q -r -s -zdp -zff -zgf -zl -zls -zp=1 -zu -dDEBUG"
# disable optimization:
# "-oc" - disable <call followed by return> to jump optimization;
# reason: "wdis" incorrectly writes "je near ptr <near_extern_label>"
#   (without "near ptr")
CL='wcl'
DA='wdis'
PC='tpc -gd -q -v -$d+,e-,g+,l+,n+ -DDEBUG;BETATEST'

_dir="$PROJDIR/src"
# replace slashes "/" with backslashes "\" for DOS compatibility
_dir="${_dir//\//\\}"
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
build_target $T_WATCOM obj ow/dointr.asm
build_target $T_WATCOM obj ow/intr.asm
build_target $T_WATCOM obj ow/i86.c
build_target $T_WATCOM obj ow/dos_.c
build_target $T_WATCOM obj ow/malloc.c
build_target $T_WATCOM obj ow/inp.asm
build_target $T_WATCOM obj ow/outp.asm
build_target $T_WATCOM obj ow/stdlib.c
build_target $T_WATCOM obj ow/memcmp.asm
build_target $T_WATCOM obj ow/memcpy.asm
build_target $T_WATCOM obj ow/memset.asm
build_target $T_WATCOM obj ow/strlen.asm
build_target $T_WATCOM obj ow/i4d.asm
build_target $T_WATCOM obj ow/i4m.asm
build_target $T_WATCOM obj ow/i8d086.asm
build_target $T_WATCOM obj ow/printf.c
build_target $T_WATCOM obj ow/stdio.c
build_target $T_DOS    obj dos/emstool.c
build_target $T_HW     obj hw/cpu.asm
build_target $T_HW     obj hw/dma.c
build_target $T_HW     obj hw/isr.asm
build_target $T_HW     obj hw/pic.c
build_target $T_HW     obj hw/sndctl_t.c
build_target $T_HW_SB  obj blaster/sbio.c
build_target $T_HW_SB  obj blaster/sbctl.c
build_target $T_HW_SB  obj blaster/detisr_.asm
build_target $T_HW_SB  obj blaster/detisr.c
build_target $T_HW_SB  obj blaster/sndisr_.asm
build_target $T_HW_SB  obj blaster/sndisr.c
build_target $T_MAIN   obj main/mixvars.c
build_target $T_MAIN   obj main/s3mvars.c
build_target $T_MAIN   obj main/loads3m.c
build_target $T_MAIN   obj main/fillvars.c
build_target $T_MAIN   obj main/voltab.c
build_target $T_MAIN   obj main/posttab.c
build_target $T_MAIN   obj main/mixer_.asm
build_target $T_MAIN   obj main/mixer.c
build_target $T_MAIN   obj main/effects.c
build_target $T_MAIN   obj main/readnote.c
build_target $T_MAIN   obj main/mixing.c
build_target $T_MAIN   obj main/filldma.c
build_target $T_MAIN   obj main/s3mplay.c
fi

if [ -n "$DJGPP" ]; then
build_target $T_TP        obj pascal/pascal.pas
build_target $T_TP        obj pascal/strutils.pas
build_target $T_WATCOM_TP obj ow/i86.pas
build_target $T_WATCOM_TP obj ow/dos_.pas
build_target $T_WATCOM_TP obj ow/malloc.pas
build_target $T_WATCOM_TP obj ow/conio.pas
build_target $T_WATCOM_TP obj ow/stdlib.pas
build_target $T_WATCOM_TP obj ow/string_.pas
build_target $T_WATCOM_TP obj ow/i4d.pas
build_target $T_WATCOM_TP obj ow/i4m.pas
build_target $T_WATCOM_TP obj ow/i8d086.pas
build_target $T_WATCOM_TP obj ow/stdio.pas
build_target $T_DOS_TP    obj dos/emstool.pas
build_target $T_HW_TP     obj hw/cpu.pas
build_target $T_HW_TP     obj hw/dma.pas
build_target $T_HW_TP     obj hw/pic.pas
build_target $T_HW_TP     obj hw/sndctl_t.pas
build_target $T_HW_SB_TP  obj blaster/sbio.pas
build_target $T_HW_SB_TP  obj blaster/sbctl.pas
build_target $T_HW_SB_TP  obj blaster/detisr.pas
build_target $T_HW_SB_TP  obj blaster/sndisr.pas
build_target $T_HW_SB_TP  obj blaster/blaster.pas
build_target $T_MAIN_TP   obj main/types.pas
build_target $T_MAIN_TP   obj main/mixtypes.pas
build_target $T_MAIN_TP   obj main/s3mtypes.pas
build_target $T_MAIN_TP   obj main/mixvars.pas
build_target $T_MAIN_TP   obj main/s3mvars.pas
build_target $T_MAIN_TP   obj main/loads3m.pas
build_target $T_MAIN_TP   obj main/fillvars.pas
build_target $T_MAIN_TP   obj main/voltab.pas
build_target $T_MAIN_TP   obj main/posttab.pas
build_target $T_MAIN_TP   obj main/mixer_.pas
build_target $T_MAIN_TP   obj main/mixer.pas
build_target $T_MAIN_TP   obj main/effvars.pas
build_target $T_MAIN_TP   obj main/effects.pas
build_target $T_MAIN_TP   obj main/readnote.pas
build_target $T_MAIN_TP   obj main/mixing.pas
build_target $T_MAIN_TP   obj main/filldma.pas
build_target $T_MAIN_TP   obj main/s3mplay.pas
build_target $T_PLAYER_TP exe player/plays3m.pas
build_target $T_PLAYER_TP exe player/smalls3m.pas
build_target $T_PLAYER_TP obj player/lines.asm
build_target $T_PLAYER_TP exe player/s3m_osci.pas
fi

cd "$PROJDIR"
