# Boot ROM
This is the new 68komputer boot ROM, mostly written in C++.

## Build Requirements
Configure CMake to cross compile. You'll have to build at least LLVM 13.0.0 or later from source, with the experimental M68k backend enabled. Also, [vasm](http://sun.hasenbraten.de/vasm/index.php) is required, particularly the M68k version with the Motorola syntax module.
