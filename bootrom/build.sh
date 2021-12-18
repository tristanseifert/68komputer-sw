#!/bin/sh

# get version info
rm -f builddate.txt commit.txt
date | tr -d '\n' > builddate.txt
git rev-parse --short HEAD | tr -d '\n' > commit.txt

# assemble the components
vasmm68k_mot -Fbin -m68000 -esc -spaces -o build/bootrom.bin -L bootrom.lst src/bootrom.68k

# build the final 1M flash ROM image, splitting it into high/low bytes
rm -f build/flash.bin

dd if=/dev/zero bs=1048576 count=1 2>/dev/null | LANG=C tr "\000" "\377" > build/flash.bin

dd if=build/bootrom.bin of=build/flash.bin seek=0 bs=1 count=32768 conv=notrunc > /dev/null 2>&1

