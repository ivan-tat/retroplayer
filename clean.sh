#!/bin/bash

saferm() {
	if [ -f "$1" ]; then rm "$1" > /dev/null; fi
}

echo "Cleaning project..."
saferm ./dos/*.obj
saferm ./dos/*.tpu
saferm ./blaster/*.tpu
saferm ./main/*.obj
saferm ./main/*.tpu
saferm ./osci/*.obj
saferm ./osci/*.exe
saferm ./player/*.obj
saferm ./player/*.exe
echo "Done."
