#include <stdint.h>
#include <stdio.h>

extern uint32_t __heap_start;
extern uint32_t __heap_end;

extern uint32_t __bss_start;
extern uint32_t __bss_end;

// helpers to clear stuff
static void clear_bss();
static void clear_heap();


/**
 * C runtime initialization (before main)
 */
void __c_init() {
  // clear various memory regions
  clear_bss();
  clear_heap();

  // TODO: call initializers
}

/**
 * Clears the BSS.
 */
static void clear_bss() {
  register uint32_t *bss = (uint32_t *) &__bss_start;
  register unsigned int size = (&__bss_end-&__bss_start);

  // clear it in blocks of 4
  for(int i = 0; i < (size / 4); i++) {
    *bss++ = 0;
  }
}

/**
 * Clears the heap.
 */
static void clear_heap() {
  register uint32_t *heap = (uint32_t *) &__heap_start;
  register unsigned int size = (&__heap_end-&__heap_start);

  // clear it in blocks of 4
  for(int i = 0; i < (size / 4); i++) {
    *heap++ = 0;
  }
}

/**
 * C runtime teardown (after main)
 */
void __c_fini() {
  // TODO: call teardown/exit functions
}
