#!/bin/bash
# This script must be run in DOS.
set -e

AS="wasm -zq"
PC='tpc -gd -q -v -$d+,e-,g+,n+ -DDEBUG;BETATEST'
export WCC="-3 -fp3 -ml -na -oi -oc -q -r -s -zdp -zff -zgf -zl -zls -zp=1 -zu -dDEBUG"
# disable optimization:
# "-oc" - disable <call followed by return> to jump optimization;
# reason: "wdis" incorrectly writes "je near ptr <near_extern_label>"
#   (without "near ptr")

compile_c() {
    local f_c="$1"
    local segname="${f_c%.*}"
    local f_obj="${f_c%.*}.obj"
    local f_lst="${f_c%.*}.lst"
    local f_tmp="${f_c%.*}.tmp"
    local f_asm="${f_c%.*}.asm"
    #segname="${segname^^}_TEXT" # such expansion is not supported by Bash-2.05b.0 in DJGPP
    segname="`echo $segname | tr a-z A-Z`_TEXT"
    wcc -nt=$segname "$f_c"
    wdis -a "$f_obj" >"$f_tmp"
    sed -r -e "s/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;\
s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$)/_DATA\1/;" "$f_tmp" >"$f_asm"
    rm -f "$f_tmp"
    $AS "$f_asm"
}

compile() {
    local f_name="$1"
    local usebin="$2"
    echo "--- $LOCDIR/$f_name ---"
    case "${f_name##*.}" in
        asm)
            $AS "$f_name"
            ;;
        c)
            compile_c "$f_name"
            ;;
        pas)
            if [ -n "$PASINC" ]; then
                $PC -u$PASINC "$f_name"
            else
                $PC "$f_name"
            fi
            ;;
    esac
}

cd pascal
LOCDIR=pascal
PASINC=''
compile pascal.pas
compile strutils.pas

cd ../watcomc
LOCDIR=watcomc
PASINC='..\pascal'
for f in \
dointr \
intr \
inp \
outp \
memcmp \
memset \
i4d \
i4m \
i8d086; do
    compile $f.asm
    compile $f.pas
done
compile printf.c
compile printf.pas

cd ../dos
LOCDIR=dos
PASINC='..\pascal;..\watcomc'
compile dosproc.c
compile dosproc.pas
compile emstool.c
compile emstool.pas

cd ../hw
LOCDIR=hw
PASINC='..\dos;..\pascal;..\watcomc'
compile cpu.asm
compile cpu.pas
compile dma.c
compile dma.pas
compile isr.asm
compile pic.c
compile pic.pas
compile sndctl_t.c
compile sndctl_t.pas

cd ../blaster
LOCDIR=blaster
PASINC='..\dos;..\hw;..\pascal;..\watcomc'
compile sbio.c
compile sbio.pas
compile sbctl.c
compile sbctl.pas
compile detisr_.asm
compile detisr.c
compile detisr.pas
compile sndisr_.asm
compile sndisr.c
compile sndisr.pas
compile blaster.pas

cd ../main
LOCDIR=main
PASINC='..\blaster;..\dos;..\hw;..\pascal;..\watcomc'
compile types.pas
compile mixtypes.pas
compile s3mtypes.pas
compile mixvars.c
compile mixvars.pas
compile s3mvars.c
compile s3mvars.pas
compile fillvars.c
compile fillvars.pas
compile voltab.c
compile voltab.pas
compile posttab.c
compile posttab.pas
compile mixer_.asm
compile mixer_.pas
compile mixer.c
compile mixer.pas
compile effvars.pas
compile effects.c
compile effects.pas
compile readnote.c
compile readnote.pas
compile mixing.c
compile mixing.pas
compile filldma.c
compile filldma.pas
compile s3mplay.c
compile s3mplay.pas

cd ../player
LOCDIR=player
PASINC='..\blaster;..\dos;..\hw;..\main;..\pascal;..\watcomc'
compile plays3m.pas
compile smalls3m.pas
compile lines.asm
compile s3m_osci.pas
cd ..
