#!/bin/bash

MODE="$1"
if [ ! -n "$MODE" ]; then
    MODE="release"
fi
echo "Mode: $MODE"

if [ "$MODE" == "clean" ]; then
    rm -rf build
    exit 0
fi

mkdir build 2> /dev/null
mkdir build/$MODE 2> /dev/null

cd build/$MODE

cmake -DCMAKE_BUILD_TYPE=$MODE ../..

make

if [ $? -eq 0 ]; then
    cp --remove-destination midi-controller ../../
    exit $?
fi
exit 1
