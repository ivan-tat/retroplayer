#!/bin/bash
# This script must be run in DOS.
set -e

AS="tasm -t"
ASW="wasm -zq"
PC='tpc -gd -q -v -$d+,e-,g+,n+ -DDEBUG;BETATEST'
export WCC="-3 -fp3 -ml -na -oi -oc -q -r -s -zdp -zff -zgf -zl -zls -zp=1 -zu"
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

cd pascal
$PC syswrap.pas

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
i4m; do
    $ASW $f.asm
    $PC $f.pas
done
compile_c printf.c
$PC $PASINC printf.pas

cd ../dos
PASINC='-u..\pascal;..\watcomc'
compile_c dosproc.c
$PC $PASINC dosproc.pas
compile_c emstool.c
$PC $PASINC emstool.pas

cd ../hw
PASINC='-u..\pascal;..\watcomc'
compile_c dma.c
$PC $PASINC dma.pas
compile_c pic.c
$PC $PASINC pic.pas

cd ../blaster
PASINC='-u..\pascal;..\watcomc;..\hw'
$ASW detisr_.asm
compile_c detisr.c
$PC $PASINC detisr.pas
compile_c sbio.c
$PC $PASINC sbio.pas
compile_c sbctl.c
$PC $PASINC sbctl.pas
$PC $PASINC blaster.pas

cd ../main
PASINC='-u..\pascal;..\watcomc;..\dos;..\hw;..\blaster'
$ASW sndisr_.asm
compile_c sndisr.c
$PC $PASINC sndisr.pas
$AS filldma.asm
$AS mixing.asm
$AS processo.asm
$AS readnote.asm
$PC $PASINC types.pas
$PC $PASINC s3mtypes.pas
compile_c voltab.c
$PC $PASINC voltab.pas
compile_c posttab.c
$PC $PASINC posttab.pas
$PC $PASINC s3mplay.pas

cd ../player
PASINC='-u..\pascal;..\watcomc;..\dos;..\hw;..\blaster;..\main'
$PC $PASINC plays3m.pas
$PC $PASINC smalls3m.pas
$AS lines.asm
$PC $PASINC s3m_osci.pas
cd ..
