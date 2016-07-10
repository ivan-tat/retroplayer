#!/bin/bash
# This script must be run in DOS.
set -e

AS="tasm -t"
ASW="wasm -zq"
PC='tpc -gd -q -v -$d+,e-,g+,n+ -DDEBUG;BETATEST'
export WCC="-3 -fp3 -ml -na -oi -q -r -s -zdp -zff -zgf -zl -zls -zp=1 -zu"

compile_c() {
	local f_c="$1"
	local segname="$2"
	local f_obj="${f_c%.*}.obj"
	local f_lst="${f_c%.*}.lst"
	local f_tmp="${f_c%.*}.tmp"
	local f_asm="${f_c%.*}.asm"
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

cd ../dos
PASINC='-u..\pascal;..\watcomc'
compile_c dosproc.c DOSPROC_TEXT
$PC $PASINC dosproc.pas
compile_c emstool.c EMSTOOL_TEXT
$PC $PASINC emstool.pas

cd ../hw
PASINC='-u..\pascal;..\watcomc'
compile_c dma.c DMA_TEXT
$PC $PASINC dma.pas
compile_c pic.c PIC_TEXT
$PC $PASINC pic.pas

cd ../blaster
PASINC='-u..\pascal;..\watcomc;..\hw'
compile_c sbio.c SBIO_TEXT
$PC $PASINC sbio.pas
compile_c sbctl.c SBCTL_TEXT
$PC $PASINC sbctl.pas
$PC $PASINC blaster.pas

cd ../main
PASINC='-u..\pascal;..\watcomc;..\dos;..\hw;..\blaster'
$AS filldma.asm
$AS mixing.asm
$AS processo.asm
$AS readnote.asm
$PC $PASINC types.pas
$PC $PASINC s3mtypes.pas
compile_c voltab.c VOLTAB_TEXT
$PC $PASINC voltab.pas
compile_c posttab.c POSTTAB_TEXT
$PC $PASINC posttab.pas
$PC $PASINC s3mplay.pas

cd ../player
PASINC='-u..\pascal;..\watcomc;..\dos;..\hw;..\blaster;..\main'
$PC $PASINC plays3m.pas
$PC $PASINC smalls3m.pas
$AS lines.asm
$PC $PASINC s3m_osci.pas
cd ..
