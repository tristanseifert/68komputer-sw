/* STARTUP(start.o) */
OUTPUT_ARCH(m68k)
/* OUTPUT_FORMAT(bin) */

/* SEARCH_DIR(.) */
/* GROUP(-lts2 -lc -lgcc) */
__DYNAMIC  =  0;

MEMORY {
  rom (rx)  : ORIGIN = 0x000000, LENGTH = 128K
  ram (rwx) : ORIGIN = 0x082000, LENGTH = 500K
}

SECTIONS {
  /* code and read-only data */
  .text : {
    /* startup code */
    *(.startup .startup.*)

    /* regular ass code */
    *(.text .text.*)

    /* constructor list */
    . = ALIGN(0x4);
     __CTOR_LIST__ = .;
    ___CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    *(.ctors)
    LONG(0)
    __CTOR_END__ = .;

    /* distructor list */
    __DTOR_LIST__ = .;
    ___DTOR_LIST__ = .;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
    *(.dtors)
     LONG(0)
    __DTOR_END__ = .;

    /* read-only data */
    *(.rodata .rodata.*)

    /* exceptions table */
    *(.gcc_except_table)

    /* initializer section */
    . = ALIGN(0x2);
    __INIT_SECTION__ = . ;
    LONG (0x4e560000)	/* linkw %fp,#0 */
    *(.init)
    SHORT (0x4e5e)	/* unlk %fp */
    SHORT (0x4e75)	/* rts */

    /* exit handlers section */
    __FINI_SECTION__ = . ;
    LONG (0x4e560000)	/* linkw %fp,#0 */
    *(.fini)
    SHORT (0x4e5e)	/* unlk %fp */
    SHORT (0x4e75)	/* rts */

    /* end of text segment */
    _etext = .;

    /* idk what this is */
    *(.lit)
  } > ram

  /* rw data */
  .data : {
    *(.got.plt) *(.got)
    *(.shdata)
    *(.data .data.*)
    _edata = .;
  } > ram

  /* non-initialized variables */
  .bss : {
    . = ALIGN(0x4);
    __bss_start = . ;
    *(.shbss)
    *(.bss .bss.*)
    *(COMMON)
    _end =  ALIGN (0x8);
    __end = _end;

    /* allocate some stack */
    . = ALIGN(0x10);
    __stack_top = .;
    . += 0x400;
    __stack = .;

    /* reserve the rest of the space for heap */
    . = ALIGN(0x10);
    __heap_start = .;
    /* . += (500K - .); */
    . += 256K;
    __heap_end = .;
  } > ram

  /* string tables */
  .stab 0 (NOLOAD) : {
    *(.stab)
  }

  .stabstr 0 (NOLOAD) : {
    *(.stabstr)
  }
}
