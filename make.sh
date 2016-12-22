#!/bin/bash
# This script must be run in DOS.
set -e

AS="tasm -t"
ASW="wasm -zq"
PC='tpc -gd -q -v -$d+,e-,g+,n+ -DDEBUG;BETATEST'
export WCC="-3 -fp3 -ml -na -oi -oc -q -r -s -zdp -zff -zgf -zl -zls -zp=1 -zu -dDEBUG"
# disable optimization:
# "-oc" - disable <call followed by return> to jump optimization;
# reason: "wdis" incorrectly writes "je near ptr <near_extern_label>"
#   (without "near ptr")

logfile() {
    local f_name="$1"
    echo "### $LOCDIR/$f_name"
}

compile_a() {
    local f_name="$1"
    local usebin="$2"
    logfile "$f_name"
    if [ "$usebin" = 'tasm' ]; then
        $AS "$f_name"
    else
        $ASW "$f_name"
    fi
}

compile_c() {
    local f_c="$1"
    local segname="${f_c%.*}"
    local f_obj="${f_c%.*}.obj"
    local f_lst="${f_c%.*}.lst"
    local f_tmp="${f_c%.*}.tmp"
    local f_asm="${f_c%.*}.asm"
    logfile "$f_c"
    #segname="${segname^^}_TEXT" # such expansion is not supported by Bash-2.05b.0 in DJGPP
    segname="`echo $segname | tr a-z A-Z`_TEXT"
    wcc -nt=$segname "$f_c"
    wdis -a "$f_obj" >"$f_tmp"
    sed -r -e "s/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;\
s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$)/_DATA\1/;" "$f_tmp" >"$f_asm"
    rm -f "$f_tmp"
    $ASW "$f_asm"
}

compile_p() {
    local f_name="$1"
    logfile "$f_name"
    if [ -n "$PASINC" ]; then
        $PC $PASINC "$f_name"
    else
        $PC "$f_name"
    fi
}

cd pascal
LOCDIR=pascal
PASINC=''
compile_p strutils.pas
compile_p syswrap.pas

cd ../watcomc
LOCDIR=watcomc
PASINC='-u..\pascal'
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
    compile_a $f.asm
    compile_p $f.pas
done
compile_c printf.c
compile_p printf.pas

cd ../dos
LOCDIR=dos
PASINC='-u..\pascal;..\watcomc'
compile_c dosproc.c
compile_p dosproc.pas
compile_c emstool.c
compile_p emstool.pas

cd ../hw
LOCDIR=hw
PASINC='-u..\pascal;..\watcomc'
compile_a cpu.asm
compile_p cpu.pas
compile_c dma.c
compile_p dma.pas
compile_a isr.asm
compile_c pic.c
compile_p pic.pas

cd ../blaster
LOCDIR=blaster
PASINC='-u..\pascal;..\watcomc;..\hw'
compile_a detisr_.asm
compile_c detisr.c
compile_p detisr.pas
compile_a sndisr_.asm
compile_c sndisr.c
compile_p sndisr.pas
compile_c sbio.c
compile_p sbio.pas
compile_c sbctl.c
compile_p sbctl.pas
compile_p blaster.pas

cd ../main
LOCDIR=main
PASINC='-u..\pascal;..\watcomc;..\dos;..\hw;..\blaster'
compile_p types.pas
compile_p mixtypes.pas
compile_p s3mtypes.pas
compile_c mixvars.c
compile_p mixvars.pas
compile_c s3mvars.c
compile_p s3mvars.pas
compile_c fillvars.c
compile_p fillvars.pas
compile_c voltab.c
compile_p voltab.pas
compile_c posttab.c
compile_p posttab.pas
compile_a mixer_.asm
compile_p mixer_.pas
compile_c mixer.c
compile_p mixer.pas
compile_p effvars.pas
compile_c effects.c
compile_p effects.pas
compile_a readnote.asm tasm
compile_p readnote.pas
compile_c mixing.c
compile_p mixing.pas
compile_c filldma.c
compile_p filldma.pas
compile_p s3mplay.pas

cd ../player
LOCDIR=player
PASINC='-u..\pascal;..\watcomc;..\dos;..\hw;..\blaster;..\main'
compile_p plays3m.pas
compile_p smalls3m.pas
compile_a lines.asm
compile_p s3m_osci.pas
cd ..
