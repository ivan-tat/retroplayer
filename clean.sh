#!/bin/bash
echo "Cleaning project..."
rm -f ./watcomc/*.obj
rm -f ./watcomc/*.tpu
rm -f ./dos/dosproc.asm
rm -f ./dos/*.obj
rm -f ./dos/*.tpu
rm -f ./blaster/*.tpu
rm -f ./main/*.obj
rm -f ./main/*.tpu
rm -f ./player/*.obj
rm -f ./player/*.exe
echo "Done."
