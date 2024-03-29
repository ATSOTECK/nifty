#!/bin/sh

if ! test -f ./bin/nifty; then
    echo "Run build.sh first!"
    exit 0
fi

cp ./bin/nifty /usr/local/bin/
echo "Nifty was installed at /usr/local/bin"
