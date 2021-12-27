ENTRY(romentry)
OUTPUT_ARCH(m68k)

MEMORY { 
    ROM (rx)  : ORIGIN = 0, LENGTH = 1M
    /* take all of the fast RAM for .data and .bss */
    RAM (rwx) : ORIGIN = 0xFFFF8000, LENGTH = 32k
}

SECTIONS {
    . = 0x000000;

    __text_start = .;
    .text : {
        *(.vectors .vectors*)
        *(.svctable .svctable*)

        . = ALIGN(4);
        *(CODE*)
        *(.text .text.*)

        . = ALIGN(4);
        *(.rodata .rodata*)
        . = ALIGN(4);
    } > ROM
    __text_end = .;

    /* location of the .data segment to copy from ROM */
    __data_rom_start = .;
    .data : AT (ADDR(.text) + SIZEOF (.text)) {
        /* start of .data in RAM */
        __data_ram_start = .;
        *(*.data .data.*)
        . = ALIGN(4);
        /* end of .data in RAM */
        __data_ram_end = .;
    } > RAM
    /* total bytes of .data to copy from ROM */
    PROVIDE(__data_rom_size = __data_ram_end - __data_ram_start);
    /* total dwords of data to copy, minus 1 (for use as a loop counter) */
    PROVIDE(__data_rom_dwords = ((__data_ram_end - __data_ram_start) / 4) - 1);

    . = ALIGN(4);

    .bss (NOLOAD) : ALIGN(4) {
        /* start of .bss in RAM */
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        /* end of .bss in RAM */
        __bss_end = .;

        /* some variables shouldn't be cleared */
        . += 4;
        . = ALIGN(4);
        *(.bssnoclr .bssnoclr.*)

        /* align to 4 bytes to be nice */
        . = ALIGN(4);
    } > RAM
    /* total bytes of .bss in RAM to clear */
    PROVIDE(__bss_size = __bss_end - __bss_start);
    /* number of dwords, minus 1, of bss (for use as a loop counter) */
    PROVIDE(__bss_dwords = ((__bss_end - __bss_start) / 4));

    /* 0x400 = space reserved for stack */
    . = ORIGIN(RAM) + LENGTH(RAM) - 0x400;
    _stack_top = .;
    . += 0x400;
    _stack_ptr = .;
}
