#!/bin/bash

cd "$(dirname "$0")"

function Test()
{
    local name=$1

    local out="$(./$name 2>/dev/null | diff -u - $name.expected)"
    if [[ -z "$out" ]]; then
        echo "ok $name"
    else
        while read line; do
            echo "# $line"
        done <<< "$out"
        echo "not ok $name"
    fi
}

echo '1..7'
Test "core"
Test "sandbox"
Test "cpp"
Test "inline"
Test "bdd"
Test "xml_reporter"
Test "tap_reporter"
