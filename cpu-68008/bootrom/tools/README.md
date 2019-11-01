This directory contains tools used for building the software.

# checksum
Calculates the checksum for an app and writes it into its header. Compile this with `clang checksum.c -o checksum` while in this directory.

# load.py
A small script that complements the load command in the monitor. Given an input binary file, a load address, and serial port, the data will be sent using the protocol described in the monitor.

Note that you will need to install the packages from `requirements.txt` to use this.
