#!/bin/sh

if ! [ -f bin/boot.bin ]; then
    ./build.sh
fi

qemu-system-i386 -hda ./bin/os.bin