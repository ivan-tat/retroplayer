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
	$ASW "$f_asm"
}

echo '*** pascal'
cd pascal
$PC strutils.pas
$PC syswrap.pas

echo '*** watcom'
cd ../watcomc
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
    $ASW $f.asm
    $PC $f.pas
done
compile_c printf.c
$PC $PASINC printf.pas

echo '*** dos'
cd ../dos
PASINC='-u..\pascal;..\watcomc'

compile_c dosproc.c
$PC $PASINC dosproc.pas

compile_c emstool.c
$PC $PASINC emstool.pas

echo '*** hw'
cd ../hw
PASINC='-u..\pascal;..\watcomc'

$ASW cpu.asm
$PC $PASINC cpu.pas

compile_c dma.c
$PC $PASINC dma.pas

compile_c pic.c
$PC $PASINC pic.pas

echo '*** blaster'
cd ../blaster
PASINC='-u..\pascal;..\watcomc;..\hw'

$ASW detisr_.asm
compile_c detisr.c
$PC $PASINC detisr.pas

$ASW sndisr_.asm
compile_c sndisr.c
$PC $PASINC sndisr.pas

compile_c sbio.c
$PC $PASINC sbio.pas

compile_c sbctl.c
$PC $PASINC sbctl.pas

$PC $PASINC blaster.pas

cd ../main
PASINC='-u..\pascal;..\watcomc;..\dos;..\hw;..\blaster'

echo '*** types'
$PC $PASINC types.pas
$PC $PASINC mixtypes.pas
$PC $PASINC s3mtypes.pas

echo '*** mixvars.c/pas'
compile_c mixvars.c
$PC $PASINC mixvars.pas

echo '*** s3mvars.c/pas'
compile_c s3mvars.c
$PC $PASINC s3mvars.pas

echo '*** fillvars.c/pas'
compile_c fillvars.c
$PC $PASINC fillvars.pas

echo '*** voltab.c'
compile_c voltab.c
$PC $PASINC voltab.pas

echo '*** posttab.c'
compile_c posttab.c
$PC $PASINC posttab.pas

echo '*** mixer_.asm'
$ASW mixer_.asm
$PC $PASINC mixer_.pas

echo '*** mixer.c'
compile_c mixer.c
$PC $PASINC mixer.pas

echo '*** effects.c'
$PC $PASINC effvars.pas
compile_c effects.c
$PC $PASINC effects.pas

echo '*** readnote.asm'
$AS readnote.asm
$PC $PASINC readnote.pas

echo '*** mixing.c'
compile_c mixing.c
$PC $PASINC mixing.pas

echo '*** filldma.c'
compile_c filldma.c
$PC $PASINC filldma.pas

echo '*** s3mplay.pas'
$PC $PASINC s3mplay.pas

echo '*** player'
cd ../player
PASINC='-u..\pascal;..\watcomc;..\dos;..\hw;..\blaster;..\main'
$PC $PASINC plays3m.pas
$PC $PASINC smalls3m.pas
$ASW lines.asm
$PC $PASINC s3m_osci.pas
cd ..
