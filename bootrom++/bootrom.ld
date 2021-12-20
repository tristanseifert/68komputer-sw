ENTRY(romentry)
OUTPUT_ARCH(m68k)

MEMORY { 
    ROM (rx)  : ORIGIN = 0, LENGTH = 1M
    RAM (rwx) : ORIGIN = 0xFFFFC000, LENGTH = 16k
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
        /* end of .bss in RAM */
        __bss_end = .;
    } > RAM
    /* total bytes of .bss in RAM to clear */
    PROVIDE(__bss_size = __bss_end - __bss_start);
    /* number of dwords, minus 1, of bss (for use as a loop counter) */
    PROVIDE(__bss_dwords = ((__bss_end - __bss_start) / 4));

    /* put the stack at the very end of RAM */
    PROVIDE(_stack_ptr = ORIGIN(RAM) + LENGTH(RAM));
}
