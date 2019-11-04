#include <stdint.h>
#include <stdio.h>

#include <tinyalloc.h>

extern uint32_t __heap_start;
extern uint32_t __heap_end;

extern uint32_t __bss_start;
extern uint32_t __end;

/**
 * C runtime initialization (before main)
 */
void __c_init() {
  // zero the bss
  uint32_t *bss = (uint32_t *) &__bss_start;

  do {
    *bss = 0;
  } while((uint32_t) bss++ <= (uint32_t) &__end);

  // zero the heap
  // uint8_t *heap = (uint8_t *) &__heap_start;
  //
  // do {
  //   *heap = 0x00;
  // } while((uint32_t) heap++ <= (uint32_t) &__heap_end);

  // initialize the heap
  ta_init(&__heap_start, &__heap_end, 256, 16, 8);

  // TODO: call initializers

  // printf("Heap from $%p to $%p\r\n", &__heap_start, &__heap_end);
}

/**
 * C runtime teardown (after main)
 */
void __c_fini() {
  // TODO: call teardown/exit functions
}
