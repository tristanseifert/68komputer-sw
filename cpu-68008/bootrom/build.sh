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

# build enhanced basic (delete everything before $8000 in output)
echo "\n\nAssembling Enhanced BASIC…"
vasmm68k_mot -Fbin -m68008 -esc -spaces -o ebasic/ebasic.bin -L ebasic/ebasic.lst ebasic/basic.68k
dd if=ebasic/ebasic.bin of=ebasic/ebasic_trimmed.bin skip=32768 bs=1 conv=notrunc

# create some ROM images
echo "\n\nCreating ROM image"
rm -f rom.bin rom_vubug.bin

dd if=/dev/zero bs=131072 count=1 | LANG=C tr "\000" "\377" > rom.bin
cp rom.bin rom_vubug.bin

dd if=bootrom.bin of=rom.bin seek=0 conv=notrunc
dd if=vubug/vubug.bin of=rom_vubug.bin seek=0 conv=notrunc

# into both ROMs, insert Enhanced BASIC at $8000
dd if=ebasic/ebasic_trimmed.bin of=rom.bin seek=32768 bs=1 conv=notrunc
dd if=ebasic/ebasic_trimmed.bin of=rom_vubug.bin seek=32768 bs=1 conv=notrunc
