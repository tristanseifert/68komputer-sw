ENTRY(romentry)

MEMORY { 
    ROM (rx)  : ORIGIN = 0, LENGTH = 1M
    RAM (rwx) : ORIGIN = 0xFFFFC000, LENGTH = 16k
}

SECTIONS {
    . = 0x000000;

    __text_start = .;
    .text : {
        *(.vectors .vectors*)
        *(.text .text.* CODE*)
        *(.rodata .rodata.*)
    } > ROM
    __text_end = .;

    __data_start = .;
    .data : ALIGN(16) {
        *(*.data .data.*)
    } > RAM AT >ROM
    __data_end = .;

    __bss_start = .;
    .bss (NOLOAD) : ALIGN(16) {
        *(.bss .bss.*)
        *(COMMON)
    } > RAM
    __bss_end = .;

    PROVIDE(_stack_ptr = ORIGIN(RAM) + LENGTH(RAM));
}
