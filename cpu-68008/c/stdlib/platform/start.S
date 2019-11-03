  .title "start.S"

  .extern main
  .extern __stack
  # .extern __bss_start
  # .extern __text_start

  .globl _start

  .align 2
  .section  .startup
  # .section  .text

_start:
  nop

  /* prepare stack and frame pointer. */
  move.l  #__stack, %sp
  # link    a6, #-8

  /* call main(int argc, char *argv[]) */
  # move.l  #0, -(sp)
  # move.l  #0, -(sp)
  # move.l  #0, -(sp)
  jsr     main
  # lea     (sp, 12), sp

  /* return to monitor */
  trap    #0
