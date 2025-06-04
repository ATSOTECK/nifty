#!/bin/sh

ARCH=$(uname -m)
echo "Building for $ARCH"

cmake -DCMAKE_BUILD_TYPE=Release -B bin
cmake --build bin

cp ./bin/nifty /usr/local/bin/
echo "Nifty was installed at /usr/local/bin"