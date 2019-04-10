#!/bin/sh
#
# Description:
#    Prepare a graphviz graph file with dependecies of selected source files.
#
# Developers:
#    Tatarinov Ivan Ivanovich, 2019, <ivan-tat@ya.ru>
#
# Debian's packages used:
#    bash | dash, coreutils, gawk, grep, sed.
#
# This is free and unencumbered software released into the public domain.
# For more information, please refer to <http://unlicense.org>.

if [ $# -ne 1 ]; then
    echo "Usage: $0 <config_file>" >&2
    exit 1
fi

. $1

echo "Source base: '"$ARG_SOURCE_DIR"' (real path '"`realpath $ARG_SOURCE_DIR`"')"

if [ $ARG_SHOW_SYSLIB_NODES -ne 0 ]; then
    tmp_nodes_sys=`mktemp`
    tmp_edges_sys=`mktemp`
fi
if [ $ARG_SHOW_SOURCE_NODES -ne 0 ]; then
    tmp_nodes=`mktemp`
    tmp_edges=`mktemp`
fi

tmp=`mktemp`

if [ $ARG_SHOW_SYSLIB_NODES -ne 0 ]; then
    tmp_includes_sys=`mktemp`
    tmp_labels_sys=`mktemp`
    tmp_names_sys=`mktemp`
fi
if [ $ARG_SHOW_SOURCE_NODES -ne 0 ]; then
    tmp_includes=`mktemp`
    tmp_labels=`mktemp`
    tmp_names=`mktemp`
fi

while read f; do
    echo "Processing file '$f'..."
    f_name="\"$f\""
    f_label="$f"
    f="$ARG_SOURCE_DIR/$f"

    # add current node
    echo "$f_name"'|'"$f_label" >> "$tmp_nodes"

    # filter included files
    grep -Ee '[[:space:]]*#include[[:space:]]+.+' "$f" > "$tmp"

    if [ $ARG_SHOW_SYSLIB_NODES -ne 0 ]; then
        grep -Ee '[[:space:]]*#include[[:space:]]+<.+>.*' "$tmp" | \
        sed -re 's/[[:space:]]*#include[[:space:]]+<(.+)>.*/\1/' > "$tmp_includes_sys"
        # add node names
        cp "$tmp_includes_sys" "$tmp_labels_sys"
        sed -re 's/(.+)/"<\1>"/' "$tmp_includes_sys"> "$tmp_names_sys"
        # add found nodes
        paste "$tmp_names_sys" "$tmp_labels_sys" -d '|' >> "$tmp_nodes_sys"
        # link current file to found nodes
        gawk '{ print $0"|"'"$f_name"' }' "$tmp_names_sys" >> "$tmp_edges_sys"
    fi
    if [ $ARG_SHOW_SOURCE_NODES -ne 0 ]; then
        grep -Ee '[[:space:]]*#include[[:space:]]+".+".*' "$tmp" | \
        sed -re 's/[[:space:]]*#include[[:space:]]+"(.+)".*/\1/' > "$tmp_includes"
        # add node names and labels
        cp "$tmp_includes" "$tmp_labels"
        sed -re 's/(.+)/"\1"/' "$tmp_includes"> "$tmp_names"
        # add found nodes and labels
        paste "$tmp_names" "$tmp_labels" -d '|' >> "$tmp_nodes"
        # link current file to found nodes
        gawk '{ print $0"|"'"$f_name"' }' "$tmp_names" >> "$tmp_edges"
    fi
done < "$ARG_FILES_LIST"

if [ $ARG_SHOW_SYSLIB_NODES -ne 0 ]; then
    rm "$tmp_includes_sys"
    rm "$tmp_labels_sys"
    rm "$tmp_names_sys"
fi
if [ $ARG_SHOW_SOURCE_NODES -ne 0 ]; then
    rm "$tmp_includes"
    rm "$tmp_labels"
    rm "$tmp_names"
fi

# process found nodes and links

if [ $ARG_SHOW_SYSLIB_NODES -ne 0 ]; then
    sort -u "$tmp_nodes_sys" > "$tmp"
    sed -re 's/(.+)\|(.+)/\1 [ label="\2" '"$NODE_FORMAT_SYS"' ];/' "$tmp" > "$tmp_nodes_sys"
    sort -u "$tmp_edges_sys" > "$tmp"
    sed -re 's/(.+)\|(.+)/\1 -> "\2" [ '"$EDGE_FORMAT_SYS"' ];/' "$tmp" > "$tmp_edges_sys"
fi

if [ $ARG_SHOW_SOURCE_NODES -ne 0 ]; then
    sort -u "$tmp_nodes" > "$tmp"
    sed -re 's/(.+)\|(.+)/\1 [ label="\2" '"$NODE_FORMAT"' ];/' "$tmp" > "$tmp_nodes"
    sort -u "$tmp_edges" > "$tmp"
    sed -re 's/(.+)\|(.+)/\1 -> "\2" [ '"$EDGE_FORMAT"' ];/' "$tmp" > "$tmp_edges"
fi

rm "$tmp"

echo "Generating graphviz file '"$ARG_GV_FILE"'..."

cat > "$ARG_GV_FILE" <<EOT
digraph g {
graph [ splines=true overlap=false rankdir=$GRAPH_RANKDIR ];
EOT

if [ $ARG_SHOW_SYSLIB_NODES -ne 0 ]; then
    cat "$tmp_nodes_sys" >> "$ARG_GV_FILE"
    cat "$tmp_edges_sys" >> "$ARG_GV_FILE"
fi
if [ $ARG_SHOW_SOURCE_NODES -ne 0 ]; then
    cat "$tmp_nodes" >> "$ARG_GV_FILE"
    cat "$tmp_edges" >> "$ARG_GV_FILE"
fi

cat >> "$ARG_GV_FILE" <<EOT
}
EOT

if [ $ARG_SHOW_SYSLIB_NODES -ne 0 ]; then
    rm "$tmp_nodes_sys"
    rm "$tmp_edges_sys"
fi
if [ $ARG_SHOW_SOURCE_NODES -ne 0 ]; then
    rm "$tmp_nodes"
    rm "$tmp_edges"
fi

echo 'Done.'
