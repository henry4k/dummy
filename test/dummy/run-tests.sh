#!/bin/sh
echo "1..$#"
for executable in "$@"; do
    name="$(basename "$executable")"
    output="$("$executable" 2>/dev/null | diff -u - "$name.expected")"
    if [ -z "$output" ]; then
        echo "ok $name"
    else
        while read line; do
            echo "# $line"
        done <<< "$output"
        echo "not ok $name"
    fi
done
