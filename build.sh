#!/bin/zsh

# Use system arch by default.
ARCH=$(uname -m)

case $1 in
arm)
    ARCH="arm64"
    ;;
intel)
    ARCH="x86_64"
    ;;
esac

cmake -DCMAKE_BUILD_TYPE=Release -DARCH=${ARCH} -B bin
cmake --build bin
