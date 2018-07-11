#!/bin/bash
# This script must be run in GNU/Linux.
#
# Make object files compatible with Pascal linker.

#. ./djgpp.sh
. ./ow.sh

disasm() {
    local f_obj="$1"
    local segname="$2"
    local f_lst="${f_obj%.*}.lst"
    local f_tmp="${f_obj%.*}.tmp"
    local f_asm="${f_obj%.*}.asm"
    f_obj="${f_obj%.obj}.o"
    wdis -l="$f_lst" "$f_obj"
    wdis -a "$f_obj" >"$f_tmp"
    sed -r -e "s/(^DGROUP[[:space:]]+GROUP[[:space:]]+)CONST,CONST2,(_DATA)/\1\2/;\
s/^CONST[2]?([[:space:]]+(SEGMENT[[:space:]]+.+*|ENDS[[:space:]]*)$)/_DATA\1/;\
s/^_TEXT([[:space:]]+(SEGMENT[[:space:]]+|ENDS[[:space:]]*))/$segname\1/;\
s/(^[[:space:]]*ASSUME[[:space:]]+.+:)_TEXT/\1$segname/g" "$f_tmp" >"$f_asm"
    rm "$f_tmp"
}

DST=tmp
mkdir -p "$DST"
cd $DST
# Watcom specific
disasm i4d.obj I4D_TEXT
disasm i4m.obj I4M_TEXT
disasm i8d086.obj I8D086_TEXT
# <conio.h>
disasm inp.obj INP_TEXT
disasm outp.obj OUTP_TEXT
# <i86.h>
disasm intr.obj INTR_TEXT
disasm dointr.obj DOINTR_TEXT
disasm enable.obj ENABLE_TEXT
disasm disable.obj DISABLE_TEXT
# <env.h>
disasm getenv.obj GETENV_TEXT
# <dos.h>
disasm dosret.obj DOSRET_TEXT
disasm d_getvec.obj D_GETVEC_TEXT
disasm d_setvec.obj D_SETVEC_TEXT
disasm error086.obj ERROR086_TEXT
disasm exv.obj EXV_TEXT
disasm exve.obj EXVE_TEXT
disasm gtime086.obj GTIME086_TEXT
disasm creatdos.obj CREATDOS_TEXT
disasm opendos.obj OPENDOS_TEXT
disasm filedos.obj FILEDOS_TEXT
disasm io086.obj IO086_TEXT
# <stdio.h>
disasm fclose.obj FCLOSE_TEXT
disasm fopen.obj FOPEN_TEXT
disasm fread.obj FREAD_TEXT
disasm fwrite.obj FWRITE_TEXT
# <stdlib.h>
disasm atoi.obj ATOI_TEXT
disasm atol.obj ATOL_TEXT
disasm istable.obj ISTABLE_TEXT
disasm strtol.obj STRTOL_TEXT
disasm errno.obj ERRNO_TEXT
disasm crwdata.obj CRWDATA_TEXT
disasm pspptr.obj PSPPTR_TEXT
# <string.h>
disasm memcmp.obj MEMCMP_TEXT
disasm memcpy.obj MEMCPY_TEXT
disasm memset.obj MEMSET_TEXT
disasm strchr.obj STRCHR_TEXT
disasm strlen.obj STRLEN_TEXT
disasm strncmp.obj STRNCMP_TEXT
disasm strncpy.obj STRNCPY_TEXT
disasm strnicmp.obj STRNICMP_TEXT
cd $OLDPWD
