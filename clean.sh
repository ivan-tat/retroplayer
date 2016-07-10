#!/bin/bash

cleandir() {
	local DIR="$1"
	echo "Cleaning $DIR ..."
    cd $DIR
	rm -f *.err
	rm -f *.exe
	rm -f *.map
	rm -f *.obj
	rm -f *.tpu
    cd $OLDPWD
}

cleandir ./hw
rm -f ./hw/dma.asm
rm -f ./hw/pic.asm
cleandir ./blaster
rm -f ./blaster/sbio.asm
rm -f ./blaster/sbctl.asm
cleandir ./dos
rm -f ./dos/dosproc.asm
rm -f ./dos/emstool.asm
cleandir ./main
rm -f ./main/voltab.asm
rm -f ./main/posttab.asm
cleandir ./pascal
cleandir ./player
cleandir ./watcomc

echo "Done."
