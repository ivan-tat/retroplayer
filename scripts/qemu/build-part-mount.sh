#!/bin/bash
# Debian packages used: coreutils, mount, util-linux, sudo.

set -e
. ./build.conf

part_mount() {
    local HDD_FILE="$1"
    local HDD_PART="$2"
    local HDD_OPTS="$3"
    local MOUNT_POINT="$HDD_FILE.p$HDD_PART.dir"
    local LOOP_DEVICE
    if [ ! -d "$MOUNT_POINT" ]; then mkdir "$MOUNT_POINT"; fi
    if [ -f "$HDD_FILE.dev" ]; then
        LOOP_DEVICE=`cat "$HDD_FILE.dev"`
        echo "Found loop-device '$LOOP_DEVICE' for '$HDD_FILE'." >&2
    else
        LOOP_DEVICE=`sudo losetup --partscan --find --show "$HDD_FILE"`
        echo "$LOOP_DEVICE" > "$HDD_FILE.dev"
        echo "Created loop-device '$LOOP_DEVICE' for '$HDD_FILE'." >&2
    fi
    sudo lsblk "$LOOP_DEVICE"
    sudo mount -o "$HDD_OPTS" "${LOOP_DEVICE}p$HDD_PART" "$MOUNT_POINT"
    echo "Mounted partition #$HDD_PART of '$HDD_FILE' as '$MOUNT_POINT'." >&2
}

part_mount "$BOOT_FILE" 1 "$BOOT_MOUNT_OPTS"
