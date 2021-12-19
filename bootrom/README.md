# Boot ROM
This directory contains the source code for the boot ROM for the 68komputer. It provides basic input/output services and functionality of a basic monitor to inspect memory and handle exceptions. It also discovers hardware connected to the system and loads their driver code.

## System Calls
To call into the ROM and take advantage of its services from a program, execute a `TRAP #15`. The trap index should be placed in `d0.w` and is used to invoke a particular function, which may in turn take further arguments. On return, `d0` contains 0 on success unless the system call defines otherwise.

Additionally, an NMI will _always_ drop into the boot ROM.

## Hardware Requirements
For the Boot ROM, really only the DUART, I2C and NVRAM chips are of interest. Additionally, the top 16K of RAM are reserved for exclusive use by the ROM.

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
