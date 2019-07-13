#!/bin/bash
#
# Sort opened files in Geany's project (http://www.geany.org)
#
# Authors / contributors:
#   Ivan Ivanovich Tatarinov <ivan-tat@ya.ru>, 2019.
#
# This is free and unencumbered software released into the public domain.
# For more information, please refer to <http://unlicense.org>.
#

declare FILE_INPUT
declare FILE_OUTPUT
declare tmp_lines
declare tmp
declare -i LINES_COUNT
declare -i LINE_START
declare -i LINE_END

_err() {
    echo "$1" >&2
}

if [[ $# -ne 1 ]]; then
    cat <<EOT
Sort opened files in Geany's project (http://www.geany.org)
This is free and unencumbered software released into the public domain.
For more information, please refer to <http://unlicense.org>.
Usage:
    `basename $0` filename
EOT
    exit
fi

FILE_INPUT=$1

tmp_lines=`mktemp`
grep -n -E -e '^FILE_NAME_[0-9]+=.+' $FILE_INPUT > $tmp_lines
LINES_COUNT=`wc -l $tmp_lines | cut -d ' ' -f 1`

tmp=`mktemp`

# Check for gaps beetwen lines

cut -d : -f 1 $tmp_lines > $tmp
LINE_START=`head -n 1 $tmp`
LINE_END=`tail -n 1 $tmp`
if [[ $LINES_COUNT -ne $((LINE_END-LINE_START+1)) ]]; then
    rm $tmp_lines $tmp
    _err 'There is a gap beetwen needed lines in input file. This is not supported.'
    exit 1
fi

# Filter lines

FILE_OUTPUT=`mktemp`
head -n $((LINE_START-1)) $FILE_INPUT > $FILE_OUTPUT
sed -E -e 's/^[0-9]+:FILE_NAME_[0-9]+=(.+)/\1/' $tmp_lines | sort -t \; -k 8 > $tmp
seq 0 $((LINES_COUNT-1)) | paste -d = - $tmp | sed -E -e 's/(.+)/FILE_NAME_\1/' >> $FILE_OUTPUT
tail -n +$((LINE_END+1)) $FILE_INPUT >> $FILE_OUTPUT
rm $tmp_lines $tmp

# Save file

mv $FILE_INPUT "$FILE_INPUT~"
mv $FILE_OUTPUT $FILE_INPUT
