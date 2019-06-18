#!/bin/bash
#
# Unmounts previously mounted partition(s) of a virtual hard disk image file.
#
# Debian packages used: coreutils, mount, util-linux, sudo.

set -e
. ./build.conf

declare HDD_FILE
declare LOOP_DEVICE

loop_find() {
	HDD_FILE="$1"
	if [ ! -f "$HDD_FILE" ]; then
		echo "File '$HDD_FILE' does not exist." >&2
		return 1
	fi
	LOOP_DEVICE=`sudo losetup --raw --noheadings --output name --associated "$HDD_FILE"|head -n 1`
}

part_umount() {
	local HDD_PART="$1"
	local HDD_MOUNT="$2"
	local HDD_DEVICE
	local MOUNT_DEVICE

	if [ -n "$LOOP_DEVICE" ]; then
		HDD_DEVICE="${LOOP_DEVICE}p$HDD_PART"
	else
		HDD_DEVICE=''
	fi

	if [ -d "$HDD_MOUNT" ]; then
		MOUNT_DEVICE=`findmnt --noheadings --output source --mountpoint "$HDD_MOUNT"||true`
		if [ -n "$MOUNT_DEVICE" ]; then
			if [ "$MOUNT_DEVICE" = "$HDD_DEVICE" ]; then
				sudo umount "$HDD_MOUNT"
				rmdir "$HDD_MOUNT"
				echo "Unmounted partition #$HDD_PART of '$HDD_FILE' from '$HDD_MOUNT'." >&2
			else
				echo "Other device '$MOUNT_DEVICE' mounted on '$HDD_MOUNT'. No unmount performed." >&2
			fi
		else
			rmdir "$HDD_MOUNT"
			echo "No devices mounted on '$HDD_MOUNT'. Mount point removed." >&2
		fi
	else
		echo "Mount point '$HDD_MOUNT' does not exist. Nothing to unmount." >&2
	fi
}

loop_detach() {
	if [ -n "$LOOP_DEVICE" ]; then
		sudo losetup --detach "$LOOP_DEVICE"
	fi
}

loop_find "$BOOT_FILE"
part_umount 1 "$BOOT_MOUNT_DIR"
loop_detach
