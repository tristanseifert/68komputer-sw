# Boot ROM
This is the new 68komputer boot ROM, mostly written in C++. This makes it much easier to develop, and with size optimizations enabled, the code is still relatively compact. In addition, several small apps are built into the ROM (currently, just a BASIC interpreter) as compressed blobs to further save space. The intention is the final boot ROM takes up no more than 128K of the 1M of flash, the rest of which can be used by the system as a flash/ROM disk.

For more information about the 68000 CPU board that this ROM is intended for, check out [these pages.](https://bookstack.trist.network/books/68komputer/chapter/68000-cpu-board)

## Features
A simple description of the boot ROM is a BIOS on steroids. It does a fair bit more than provide IO services; it also handles device discovery and hardware inventory.

### System Calls
To call into the ROM and take advantage of its services from a program, execute a `TRAP #15`. The trap index should be placed in `d0.w` and is used to invoke a particular function, which may in turn take further arguments. On return, `d0` contains 0 on success unless the system call defines otherwise.

Additionally, an NMI will _always_ drop into the boot ROM.

### Hardware Requirements
For the Boot ROM, really only the DUART, I2C and NVRAM chips are of interest. Additionally, the top 16K of RAM are reserved for exclusive use by the ROM.

**TODO:** This is may be out-of-date

- $FFC000 - $FFFAFF: Reserved for .bss/.data of runtime
- $FFFB00 - $FFFCFF: Interrupt vectors
    - Each vector is allocated 8 bytes; sufficient for an absolute 32-bit jump to a handler routine.
    - Vector assignments are as follows (multiply by 8 and add to base to get its address):
        - 0: Bus error
        - 1: Address error
        - 2: Illegal instruction
        - 3: Divide by zero
        - 4: CHK instruction
        - 5: TRAPV instruction
        - 6: Privilege violation
        - 7: Trace interrupt
        - 8: Line 1010 Emulator
        - 9: Line 1111 Emulator
        - 10: Uninitialized interrupt vector
        - 11: Spurious interrupt
        - 12: Level 1 autovector
        - 13: Level 2 autovector
        - 14: Level 3 autovector
        - 15..29: TRAP vector 0-14
        - 30: Reserved
        - 31..63: User interrupt vectors (starting with vector $40)
- $FFFE00 - $FFFFFF: Supervisor stack

## Build Requirements
Configure CMake to cross compile. You'll have to build at least LLVM 13.0.0 or later from source, with the experimental M68k backend enabled. Also, [vasm](http://sun.hasenbraten.de/vasm/index.php) is required, particularly the M68k version with the Motorola syntax module.
