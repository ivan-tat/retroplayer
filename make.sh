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
$ASW dointr.asm
$PC dointr.pas
$ASW inp.asm
$PC inp.pas
$ASW intr.asm
$PC intr.pas
$ASW memcmp.asm
$PC memcmp.pas
$ASW outp.asm
$PC outp.pas

cd ../dos
compile_c dosproc.c DOSPROC_TEXT
$PC '-u..\watcomc' dosproc.pas
compile_c emstool.c EMSTOOL_TEXT
$PC '-u..\pascal;..\watcomc' emstool.pas

cd ../hw
compile_c dma.c DMA_TEXT
$PC '-u..\pascal;..\watcomc' dma.pas

cd ../blaster
compile_c sbio.c SBIO_TEXT
$PC '-u..\pascal;..\watcomc;..\hw' sbio.pas
$PC '-u..\pascal;..\watcomc;..\hw' blaster.pas

cd ../main
$AS filldma.asm
$AS mixing.asm
$AS processo.asm
$AS readnote.asm
$AS volume.asm
$PC '-U..\pascal;..\watcomc;..\dos;..\hw;..\blaster' s3mplay.pas

cd ../player
$PC '-U..\pascal;..\watcomc;..\dos;..\hw;..\blaster;..\main' plays3m.pas
$PC '-U..\pascal;..\watcomc;..\dos;..\hw;..\blaster;..\main' smalls3m.pas
$AS lines.asm
$PC '-U..\pascal;..\watcomc;..\dos;..\hw;..\blaster;..\main' s3m_osci.pas
cd ..
