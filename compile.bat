@echo off
cls
set PC=tpc -q
set AS=tasm -t
cd blaster
%PC% blaster.pas
cd ..\main
%AS% dosproc.asm
%AS% ems4fct.asm
%AS% filldma.asm
%AS% mixing.asm
%AS% processo.asm
%AS% readnote.asm
%AS% volume.asm
%PC% emstool.pas
%PC% -U..\blaster s3mplay.pas
cd ..
%PC% -Ublaster;main smalls3m
cd osci
%AS% lines.asm
%PC% -U..\blaster;..\main s3m_osci
cd ..\player
%AS% dosproc.asm
%PC% -U..\blaster;..\main plays3m
cd ..
