#!/bin/bash

cd "$(dirname "$0")"
GenStubsScript='../../extra/gen_stubs.py'

function Compare
{
    local name="$1"
    local expected="$2"
    local actual="$3"

    local out="$(diff -u $actual $expected)"
    if [[ -z "$out" ]]; then
        echo "ok $name"
    else
        while read line; do
            echo "# $line"
        done <<< "$out"
        echo "not ok $name"
    fi
}

function Test()
{
    local name="$1"

    mkdir tmp
    cd tmp

    "../$GenStubsScript" "../${name}.h"
    Compare "${name}_stub.h" "../${name}_stub.h" "${name}_stub.h"
    Compare "${name}_stub.c" "../${name}_stub.c" "${name}_stub.c"

    cd ..
    rm -rf tmp
}

echo '1..4'
Test 'empty'
Test 'simple'
