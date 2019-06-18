#!/bin/bash
#
# Mounts partition(s) of a virtual hard disk file.
#
# Debian packages used: coreutils, mount, util-linux, sudo.

set -e
. ./build.conf

declare HDD_FILE
declare LOOP_DEVICE

loop_attach() {
	HDD_FILE="$1"
	if [ ! -f "$HDD_FILE" ]; then
		echo "File '$HDD_FILE' does not exist." >&2
		return 1
	fi
	LOOP_DEVICE=`sudo losetup --raw --noheadings --output name --associated "$HDD_FILE"|head -n 1`
	if [ -z "$LOOP_DEVICE" ]; then
		LOOP_DEVICE=`sudo losetup --partscan --show --find "$HDD_FILE"`
	fi
}

part_mount() {
	local HDD_PART="$1"
	local HDD_MOUNT="$2"
	local HDD_OPTS="$3"
	local MOUNT_DEVICE
	local HDD_DEVICE
	local do_mount
	HDD_DEVICE="${LOOP_DEVICE}p$HDD_PART"
	do_mount=1
	if [ -d "$HDD_MOUNT" ]; then
		MOUNT_DEVICE=`findmnt --noheadings --output source --mountpoint "$HDD_MOUNT"||true`
		if [ -n "$MOUNT_DEVICE" ]; then
			if [ "$MOUNT_DEVICE" == "$HDD_DEVICE" ]; then
				echo "Partition #$HDD_PART of '$HDD_FILE' is already mounted on '$HDD_MOUNT'." >&2
			else
				echo "Other device '$MOUNT_DEVICE' is already mounted on '$HDD_MOUNT'. No mount performed." >&2
			fi
			do_mount=0
		fi
	else
		mkdir "$HDD_MOUNT"
	fi
	if [ $do_mount -ne 0 ]; then
		sudo mount -o "$HDD_OPTS" "$HDD_DEVICE" "$HDD_MOUNT"
		echo "Mounted partition #$HDD_PART of '$HDD_FILE' on '$HDD_MOUNT'." >&2
	fi
}

loop_attach "$BOOT_FILE"
part_mount 1 "$BOOT_MOUNT_DIR" "$BOOT_MOUNT_OPTS"
