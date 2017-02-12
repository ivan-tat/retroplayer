#!/bin/bash
# This script must be run in DOS.
#
# Make object files compatible with Pascal linker.

. ./djgpp.sh
. ./watcom.sh

disasm() {
	local f_obj="$1"
	local segname="$2"
	local f_lst="${f_obj%.*}.lst"
	local f_tmp="${f_obj%.*}.tmp"
	local f_asm="${f_obj%.*}.asm"
	wdis -l="$f_lst" "$f_obj"
	wdis -a "$f_obj" >"$f_tmp"
	sed -r -e "s/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;\
s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$)/_DATA\1/;\
s/^_TEXT([[:space:]]+(SEGMENT[[:space:]]+|ENDS[[:space:]]*))/$segname\1/;\
s/(^[[:space:]]*ASSUME[[:space:]]+.+:)_TEXT/\1$segname/g" "$f_tmp" >"$f_asm"
	rm "$f_tmp" "$f_obj"
}

DST=tmp
mkdir -p "$DST"
cd $DST
disasm intr.obj INTR_TEXT
disasm dointr.obj DOINTR_TEXT
disasm dosmem.obj DOSMEM_TEXT
disasm memcmp.obj MEMCMP_TEXT
disasm memcpy.obj MEMCPY_TEXT
disasm memset.obj MEMSET_TEXT
disasm strlen.obj STRLEN_TEXT
disasm inp.obj INP_TEXT
disasm outp.obj OUTP_TEXT
disasm i4d.obj I4D_TEXT
disasm i4m.obj I4M_TEXT
disasm i8d086.obj I8D086_TEXT
cd $OLDPWD
