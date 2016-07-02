#!/bin/bash

cleandir() {
	local DIR="$1"
	echo "Cleaning $DIR ..."
	rm -f "$DIR"/*.err
	rm -f "$DIR"/*.exe
	rm -f "$DIR"/*.map
	rm -f "$DIR"/*.obj
	rm -f "$DIR"/*.tpu
}

cleandir ./blaster
cleandir ./dos
rm -f ./dos/dosproc.asm
rm -f ./dos/emstool.asm
cleandir ./main
cleandir ./pascal
cleandir ./player
cleandir ./watcomc

echo "Done."
