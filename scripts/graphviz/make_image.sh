#!/bin/sh
#
# Description:
#    Make an image file of prepared graph with graphviz.
#
# Developers:
#    Tatarinov Ivan Ivanovich, 2019, <ivan-tat@ya.ru>
#
# Debian's packages used:
#    bash | dash, coreutils, graphviz.
#
# This is free and unencumbered software released into the public domain.
# For more information, please refer to <http://unlicense.org>.

if [ $# -ne 1 ]; then
    echo "Usage: $0 <config_file>" >&2
    exit 1
fi

. $1

echo "Generating output file '"$ARG_GRAPH_FILE"'..."

dot -T$ARG_GRAPH_FORMAT "$ARG_GV_FILE" > "$ARG_GRAPH_FILE"

echo 'Done.'
