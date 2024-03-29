#!/bin/sh

ARCH=$(uname -m)
echo "Building for $ARCH"

cmake -DCMAKE_BUILD_TYPE=Release -B bin
cmake --build bin
