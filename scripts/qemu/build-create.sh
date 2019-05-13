#!/bin/bash
# Debian packages used: coreutils, mount, util-linux, e2fsprogs, parted, sudo.

set -e
. ./build.conf

mk_boot() {
    local HDD_FILE="$1"
    local HDD_SIZE="$2"
    local HDD_LABEL="$3"
    local LOOP_DEVICE
    if [ ! -f "$HDD_FILE" ]; then
        truncate "$HDD_FILE" --size "$HDD_SIZE"
        LOOP_DEVICE=`sudo losetup --find --show "$HDD_FILE"`
        sudo parted "$LOOP_DEVICE" mktable msdos
        sudo parted "$LOOP_DEVICE" mkpart primary 2048s 100%
        sudo parted "$LOOP_DEVICE" set 1 boot on
        sudo mke2fs -t ext4 -L "$HDD_LABEL" "${LOOP_DEVICE}p1"
        sudo parted "$LOOP_DEVICE" print
        sudo losetup -d "$LOOP_DEVICE"
        echo "Created HDD image file '$HDD_FILE'." >&2
    else
        echo "HDD image file '$HDD_FILE' is already created." >&2
    fi
}

mk_swap() {
    local HDD_FILE="$1"
    local HDD_SIZE="$2"
    local HDD_LABEL="$3"
    local LOOP_DEVICE
    if [ ! -f "$HDD_FILE" ]; then
        truncate "$HDD_FILE" --size "$HDD_SIZE"
        LOOP_DEVICE="`sudo losetup --find --show "$HDD_FILE"`"
        sudo parted "$LOOP_DEVICE" mktable msdos
        sudo parted "$LOOP_DEVICE" mkpart primary 2048s 100%
        sudo mkswap --label "$HDD_LABEL" "${LOOP_DEVICE}p1"
        sudo parted "$LOOP_DEVICE" print
        sudo losetup -d "$LOOP_DEVICE"
        echo "Created HDD image file '$HDD_FILE'." >&2
    else
        echo "HDD image file '$HDD_FILE' is already created." >&2
    fi
}

mk_boot "$BOOT_FILE" "$BOOT_SIZE" "$BOOT_LABEL"
mk_swap "$SWAP_FILE" "$SWAP_SIZE" "$SWAP_LABEL"
