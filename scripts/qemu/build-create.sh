#!/bin/bash
#
# Creates virtual hard disk image files.
#
# Debian packages used: coreutils, mount, util-linux, e2fsprogs, parted, sudo.

set -e
. ./build.conf

mk_boot() {
	local HDD_FILE="$1"
	local HDD_SIZE="$2"
	local HDD_LABEL="$3"
	local LOOP_DEVICE
	local tmp
	if [ -f "$HDD_FILE" ]; then
		echo "HDD image file '$HDD_FILE' is already created." >&2
	else
		truncate "$HDD_FILE" --size "$HDD_SIZE"
		tmp=`mktemp`
		cat > "$tmp" <<-EOF
			#/bin/sh
			set -e
			LOOP_DEVICE=\`losetup --show --find "$HDD_FILE"\`
			parted "\$LOOP_DEVICE" <<-EOS
				mktable msdos
				mkpart primary 2048s 100%
				set 1 boot on
			EOS
			mke2fs -t ext4 -L "$HDD_LABEL" "\${LOOP_DEVICE}p1"
			parted "\$LOOP_DEVICE" print
			losetup --detach "\$LOOP_DEVICE"
		EOF
		chmod +x "$tmp"
		sudo "$tmp"
		rm -f "$tmp"
		echo "Created HDD image file '$HDD_FILE'." >&2
	fi
}

mk_swap() {
	local HDD_FILE="$1"
	local HDD_SIZE="$2"
	local HDD_LABEL="$3"
	local LOOP_DEVICE
	local tmp
	if [ -f "$HDD_FILE" ]; then
		echo "HDD image file '$HDD_FILE' is already created." >&2
	else
		truncate "$HDD_FILE" --size "$HDD_SIZE"
		tmp=`mktemp`
		cat > "$tmp" <<-EOF
			#/bin/sh
			set -e
			LOOP_DEVICE=\`losetup --show --find "$HDD_FILE"\`
			parted "\$LOOP_DEVICE" <<-EOS
				mktable msdos
				mkpart primary 2048s 100%
			EOS
			mkswap --label "$HDD_LABEL" "\${LOOP_DEVICE}p1"
			parted "\$LOOP_DEVICE" print
			losetup --detach "\$LOOP_DEVICE"
		EOF
		chmod +x "$tmp"
		sudo "$tmp"
		rm -f "$tmp"
		echo "Created HDD image file '$HDD_FILE'." >&2
	fi
}

mk_boot "$BOOT_FILE" "$BOOT_SIZE" "$BOOT_LABEL"
mk_swap "$SWAP_FILE" "$SWAP_SIZE" "$SWAP_LABEL"
