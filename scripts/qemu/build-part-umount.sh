#!/bin/bash
# Debian packages used: coreutils, mount, grep, sudo.

set -e
. ./build.conf

part_umount() {
    local HDD_FILE="$1"
    local HDD_PART="$2"
    local MOUNT_POINT="$HDD_FILE.p$HDD_PART.dir"
    local LOOP_DEVICE
    if [ -f "$HDD_FILE.dev" ]; then
        LOOP_DEVICE=`cat "$HDD_FILE.dev"`
        echo "Found loop-device '$LOOP_DEVICE' for '$HDD_FILE'." >&2
        if sudo mount | grep -E -e "^${LOOP_DEVICE}p$HDD_PART on `pwd`/$MOUNT_POINT" > /dev/null; then
            sudo umount "$MOUNT_POINT"
            echo "Unmounted '$HDD_FILE' from '$MOUNT_POINT'." >&2
        fi
        sudo losetup -d "$LOOP_DEVICE"
        # FIXME: bad code:
        if sudo losetup -j "$HDD_FILE" | grep -E -e "^${LOOP_DEVICE}p$HDD_PART:" > /dev/null; then
            echo "Skipped removing loop-device '$LOOP_DEVICE' (loop-device is in use)." >&2
        else
            rm "$HDD_FILE.dev"
            echo "Removed loop-device '$LOOP_DEVICE'." >&2
        fi
    fi
    if [ -d "$MOUNT_POINT" ]; then
        rmdir "$MOUNT_POINT"
        echo "Removed '$MOUNT_POINT' directory." >&2
    fi
}

part_umount "$BOOT_FILE" 1
