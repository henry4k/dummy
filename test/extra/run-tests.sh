#!/bin/sh
GenStubsScript="$1"

function Compare
{
    name="$1"
    expected="$2"
    actual="$3"

    output="$(diff -u "$actual" "$expected")"
    if [ -z "$output" ]; then
        echo "ok $name"
    else
        while read line; do
            echo "# $line"
        done <<< "$output"
        echo "not ok $name"
    fi
}

function Test()
{
    testName="$1"

    mkdir tmp
    cd tmp

    "../$GenStubsScript" "../${testName}.h"
    Compare "${testName}_stub.h" "../${testName}_stub.h" "${testName}_stub.h"
    Compare "${testName}_stub.c" "../${testName}_stub.c" "${testName}_stub.c"

    cd ..
    rm -rf tmp
}

echo '1..4'
Test 'empty'
Test 'simple'
