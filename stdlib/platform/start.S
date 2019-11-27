  .title "start.S"

  .extern main
  .extern __stack

  .extern __c_init
  .extern __c_fini

  .extern __bss_start
  .extern __bss_end

  # .extern __text_start

  .globl _start

  .align 2
  .section  .text.startup

_start:
  /* prepare stack and frame pointer. */
  lea     __stack, %sp

  /* set up the CRT */
  jsr     __c_init

  /* call main(int argc, char *argv[]) */
  moveq   #0, %d0

  link    %fp, #-12
  move.l  %d0, -(%sp)
  move.l  %d0, -(%sp)
  move.l  %d0, -(%sp)
  jsr     main
  unlk    %fp

  /* call the runtime teardown */
  jsr     __c_fini

  /* return to monitor */
0:
  trap    #0
