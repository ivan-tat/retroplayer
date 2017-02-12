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

cleandir ./blaster
rm -f ./blaster/detisr.asm
rm -f ./blaster/sbctl.asm
rm -f ./blaster/sbio.asm
rm -f ./blaster/sndisr.asm
cleandir ./dos
rm -f ./dos/dosproc.asm
rm -f ./dos/emstool.asm
cleandir ./hw
rm -f ./hw/dma.asm
rm -f ./hw/pic.asm
rm -f ./hw/sndctl_t.asm
cleandir ./main
rm -f ./main/effects.asm
rm -f ./main/filldma.asm
rm -f ./main/fillvars.asm
rm -f ./main/loads3m.asm
rm -f ./main/mixer.asm
rm -f ./main/mixing.asm
rm -f ./main/mixvars.asm
rm -f ./main/posttab.asm
rm -f ./main/readnote.asm
rm -f ./main/s3mplay.asm
rm -f ./main/s3mvars.asm
rm -f ./main/voltab.asm
cleandir ./pascal
cleandir ./player
cleandir ./watcomc
rm -f ./watcomc/dos_.asm
rm -f ./watcomc/i86.asm
rm -f ./watcomc/malloc.asm
rm -f ./watcomc/printf.asm

echo "Done."
