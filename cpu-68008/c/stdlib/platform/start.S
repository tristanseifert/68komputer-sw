  .title "start.S"

  .extern main
  .extern __stack

  .extern __c_init
  .extern __c_fini
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

  /* call the runtime initializer */
  jsr     __c_init

  /* call main(int argc, char *argv[]) */
  link    %a6, #-12
  move.l  #0, -(%sp)
  move.l  #0, -(%sp)
  move.l  #0, -(%sp)
  jsr     main
  unlk    %a6

  /* call the runtime teardown */
  jsr     __c_fini

  /* return to monitor */
  trap    #0
