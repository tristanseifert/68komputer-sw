#!/bin/sh

# add date
rm -f builddate.txt
date > builddate.txt

# build loader
echo "Assembling loader..."
vasmm68k_mot -Fbin -m68008 -spaces -o bootrom.bin -L bootrom.lst bootrom.68k

# combine them into a rom image
echo "\n\nCreating ROM image"
rm -f rom.bin
dd if=/dev/zero bs=131072 count=1 | LANG=C tr "\000" "\377" > rom.bin
dd if=bootrom.bin of=rom.bin seek=0 conv=notrunc
