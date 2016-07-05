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
cleandir ./blaster
rm -f ./blaster/sbio.asm
cleandir ./dos
rm -f ./dos/dosproc.asm
rm -f ./dos/emstool.asm
cleandir ./main
cleandir ./pascal
cleandir ./player
cleandir ./watcomc

echo "Done."
