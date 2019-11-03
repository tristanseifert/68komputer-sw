#!/bin/sh

# get the current date
rm -f builddate.txt
date > builddate.txt

# build enhanced basic
echo "Assembling Enhanced BASIC"
vasmm68k_mot -Fbin -m68008 -esc -spaces -o ebasic/ebasic.bin -L ebasic/ebasic.lst ebasic/basic.68k
dd if=ebasic/ebasic.bin of=ebasic/ebasic_trimmed.bin skip=983040 bs=1 conv=notrunc

# build the apps directory
echo "\n\nCompressing apps"
lz4 -9 -f --no-frame-crc ebasic/ebasic_trimmed.bin apps/ebasic.bin.lz4

echo "\n\nAssembling apps directory"
vasmm68k_mot -Fbin -m68008 -esc -spaces -o apps/directory.bin -L apps/directory.lst apps/directory.68k

# build VUBUG
echo "\n\nAssembling VUBUG"
vasmm68k_mot -Fbin -m68008 -esc -spaces -o vubug/vubug.bin -L vubug/vubug.lst vubug/VUBUG.68k

# create the ROM images
echo "\n\nCreating ROM image"
rm -f rom_vubug.bin

dd if=/dev/zero bs=131072 count=1 | LANG=C tr "\000" "\377" > rom_vubug.bin

dd if=vubug/vubug.bin of=rom_vubug.bin seek=0 bs=1 count=32768 conv=notrunc
dd if=apps/directory.bin of=rom_vubug.bin seek=32768 bs=1 conv=notrunc
