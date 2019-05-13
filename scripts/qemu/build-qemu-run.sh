#!/bin/bash
# Debian packages used: qemu-system-x86, sudo.

set -e
. ./build.conf

qemu-system-i386 -enable-kvm -m 128M \
-drive file="$BOOT_FILE",index=0,media=disk,format=raw,$BOOT_QEMU_FLAGS \
-drive file="$SWAP_FILE",index=1,media=disk,format=raw,$SWAP_QEMU_FLAGS \
-boot order=c \
-device $QEMU_NET_DEVICE,netdev=$QEMU_NET_ID \
-netdev user,id=$QEMU_NET_ID,$QEMU_NET_FLAGS
