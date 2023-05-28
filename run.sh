#!/bin/sh

if ! [ -f bin/boot.bin ]; then
    ./build.sh
fi

qemu-system-x86_64 -hda ./bin/os.bin