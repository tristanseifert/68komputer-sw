;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Helper methods for setting up a stack frame on return to a program.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    public resetreason
    public _ZN8shellcmd8Register11gReturnRegsE
    public _stack_ptr

    public rominit

.section code,code

    public shell_exec_handler, shell_exec_reset

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Set up a stack frame to return to the specified user function. The stack is set up so that if
;; the user function returns, it will jump back to the entry point of the ROM.
;;
;; This function has a custom register calling convention. The function address should be placed
;; in d0.
shell_exec_handler:
    move        #$2700, sr
    move.l      #_stack_ptr, sp

    ; return to a rom init stub
    pea         ExecReturnHandler

    ; set up an exception frame to return to the user code
    move.l      d0, -(sp)
    move.w      #$2000, -(sp) ; supervisor mode, enable all interrupts

    ; clear registers
    moveq       #0, d0
    moveq       #0, d1
    moveq       #0, d2
    moveq       #0, d3
    moveq       #0, d4
    moveq       #0, d5
    moveq       #0, d6
    moveq       #0, d7
    move.l      d0, a0
    move.l      d0, a1
    move.l      d0, a2
    move.l      d0, a3
    move.l      d0, a4
    move.l      d0, a5
    move.l      d0, a6

    ; execute user code
    rte

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
shell_exec_reset:
    move        #$2700, sr
    bra         romentry

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Helper method that sets up the reset reason and jumps to the initialization code when a user
;; exec routine returns.
ExecReturnHandler:
    ; save current registers
    movem.l     d0-d7/a0-a7, (_ZN8shellcmd8Register11gReturnRegsE).w
    clr.l       (_ZN8shellcmd8Register11gReturnRegsE+(4*16)).w
    move.w      sr, (_ZN8shellcmd8Register11gReturnRegsE+(4*17)).w

    ; XXX: clear console receive buffers

    ; do fake reset
    move.b      #2, (resetreason).w
    bra         rominit
