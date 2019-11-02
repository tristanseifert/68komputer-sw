#!/bin/sh
echo "Assembling 68681 IO test"
vasmm68k_mot -Fbin -m68008 -esc -spaces -o 68681_io.bin -L 68681_io.lst 68681_io.68k

echo "Assembling DS1244 Test"
vasmm68k_mot -Fbin -m68008 -esc -spaces -o ds1244_test.bin -L ds1244_test.lst ds1244_test.68k
vasmm68k_mot -Fbin -m68008 -esc -spaces -o ds1244_set.bin -L ds1244_set.lst ds1244_set.68k
