#!/bin/bash
#
# Description:
#    Prepare a source files list for dependencies graph drawing.
#
# Developers:
#    Tatarinov Ivan Ivanovich, 2019, <ivan-tat@ya.ru>
#
# Debian's packages used:
#    bash, coreutils, findutils, sed.
#
# This is free and unencumbered software released into the public domain.
# For more information, please refer to <http://unlicense.org>.

if [ $# -ne 1 ]; then
    echo "Usage: $0 <config_file>" >&2
    exit 1
fi

. $1

declare -i count
count=0
arguments=''
if [ $ARG_FIND_H_FILES -ne 0 ]; then
    arguments="-name '*.h'"
    let count=$count+1
fi
if [ $ARG_FIND_C_FILES -ne 0 ]; then
    if [ $count -ne 0 ]; then
        arguments="$arguments -o "
    fi
    arguments="${arguments}-name '*.c'"
    let count=$count+1
fi
if [ $count -gt 1 ]; then
    arguments="\( $arguments \)"
fi

cd "$ARG_SOURCE_DIR"
tmp="$OLDPWD/make_list.tmp"
cat > "$tmp" <<EOF
#!/bin/sh
find . $arguments -a \( -type f -o -type l \) | sed -re 's/^\.\///' | sort > "$OLDPWD/$ARG_FILES_LIST"
EOF
chmod +x "$tmp"
"$tmp"
rm "$tmp"
cd "$OLDPWD"
