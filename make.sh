#!/bin/bash
# This script must be run in DOS.
AS="tasm -t"
PC="tpc -q"
cd dos
$AS dosproc.asm
$AS ems4fct.asm
$PC emstool.pas
cd ../blaster
$PC blaster.pas
cd ../main
$AS filldma.asm
$AS mixing.asm
$AS processo.asm
$AS readnote.asm
$AS volume.asm
$PC '-U..\dos;..\blaster' s3mplay.pas
cd ..
$PC '-Udos;blaster;main' smalls3m
cd osci
$AS lines.asm
$PC '-U..\dos;..\blaster;..\main' s3m_osci
cd ../player
$PC '-U..\dos;..\blaster;..\main' plays3m
cd ..
