#!/bin/bash
# Debian packages used: qemu-system-x86, sudo.

set -e
. ./build.conf

sudo qemu-system-i386 -enable-kvm -m 512M \
-drive file="$BOOT_FILE",index=0,media=disk,format=raw,$BOOT_QEMU_FLAGS \
-drive file="$SWAP_FILE",index=1,media=disk,format=raw,$SWAP_QEMU_FLAGS \
-cdrom "$CDROM_FILE" \
-boot order=d
