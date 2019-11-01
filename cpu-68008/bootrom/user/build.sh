#!/bin/sh
echo "Assembling 68681 IO test"
vasmm68k_mot -Fbin -m68008 -esc -spaces -o 68681_io.bin -L 68681_io.lst 68681_io.68k
