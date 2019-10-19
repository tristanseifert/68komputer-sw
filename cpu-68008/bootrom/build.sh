#!/bin/sh

# add date
rm -f builddate.txt
date > builddate.txt

# build loader
echo "Assembling loader..."
vasmm68k_mot -Fbin -m68008 -esc -spaces -o bootrom.bin -L bootrom.lst bootrom.68k

# build VUBUG
echo "\n\nAssembling VUBUG..."
vasmm68k_mot -Fbin -m68008 -esc -spaces -o vubug/vubug.bin -L vubug/vubug.lst vubug/VUBUG.68k

# create some ROM images
echo "\n\nCreating ROM image"
rm -f rom.bin rom_vubug.bin

dd if=/dev/zero bs=131072 count=1 | LANG=C tr "\000" "\377" > rom.bin
cp rom.bin rom_vubug.bin

dd if=bootrom.bin of=rom.bin seek=0 conv=notrunc
dd if=vubug/vubug.bin of=rom_vubug.bin seek=0 conv=notrunc
