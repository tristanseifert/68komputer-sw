;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Low level ISR handlers
;
; Implementations of the assembly interrupt and exception handlers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   macro        GenerateReservedVec
   public       isr_reserved_\1
isr_reserved_\1:
    moveq       #$\1, d0
    bra         isr_reserved_common
    endm

.section code,code
    public isr_reserved, isr_bus_error, isr_address_error, isr_illegal_instruction
    public isr_zero_divide, isr_chk, isr_trapv, isr_privilege_violation
    public isr_trace, isr_line1010, isr_line1111
    public isr_irq_uninitialized, isr_irq_spurious, isr_irq_unhandled_avec1
    public isr_irq_unhandled_avec2, isr_irq_unhandled_avec3

; a reserved vector was triggered (should never happen)
isr_reserved:
    moveq       #-1, d0

isr_reserved_common:
    bra         isr_reserved

; CPU encountered a bus error during the most recent access
isr_bus_error:
    ; save registers and bus error frame
    movem.l     d0-d7/a0-a6, (exception_regs).w

    lea         (berrframe).w, a0
    move.l      (sp), (a0)+
    move.l      4(sp), (a0)+
    move.l      8(sp), (a0)+
    move.w      12(sp), (a0)

    ; is /BERR being caught?
    tst.b       (catchberr).w
    beq.s       .catch

    ; it's not, so enter the exception handler
.fatal:
    ; TODO: implement
    rte

    ; expecting this error and it should be caught
.catch:
    st.b        (hasberr).w
    rte

; A word or longword access to an odd address was attempted.
isr_address_error:
    rte

; Unknown opcode
isr_illegal_instruction:
    rte

; Divide by zero
isr_zero_divide:
    rte

; CHK instruction
isr_chk:
    rte
; TRAPV instruction
isr_trapv:
    rte

; Privilege violation
isr_privilege_violation:
    rte

; Instruction executed with trace bit set
isr_trace:
    rte

; An opcode beginning with $A was encountered
isr_line1010:
    rte
; An opcode beginning with $F was encountered
isr_line1111:
    rte

; A peripheral with an uninitialized vector register acknowledged an interrupt
isr_irq_uninitialized:
    rte
; A spurious interrupt was received (nobody acknowledged it)
isr_irq_spurious:
    rte

; A level 1 autovector irq was received
isr_irq_unhandled_avec1:
    rte
; A level 2 autovector irq was received
isr_irq_unhandled_avec2:
    rte
; A level 3 autovector irq was received
isr_irq_unhandled_avec3:
    rte

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Default irq/spurious irq handlers

; Default trap handlers; these don't do anything but raise an exception
    GenerateReservedVec         20
    GenerateReservedVec         21
    GenerateReservedVec         22
    GenerateReservedVec         23
    GenerateReservedVec         24
    GenerateReservedVec         25
    GenerateReservedVec         26
    GenerateReservedVec         27
    GenerateReservedVec         28
    GenerateReservedVec         29
    GenerateReservedVec         2A
    GenerateReservedVec         2B
    GenerateReservedVec         2C
    GenerateReservedVec         2D
    GenerateReservedVec         2E

; default user interrupt vectors
    GenerateReservedVec         40
    GenerateReservedVec         41
    GenerateReservedVec         42
    GenerateReservedVec         43
    GenerateReservedVec         44
    GenerateReservedVec         45
    GenerateReservedVec         46
    GenerateReservedVec         47
    GenerateReservedVec         48
    GenerateReservedVec         49
    GenerateReservedVec         4A
    GenerateReservedVec         4B
    GenerateReservedVec         4C
    GenerateReservedVec         4D
    GenerateReservedVec         4E
    GenerateReservedVec         4F
    GenerateReservedVec         50
    GenerateReservedVec         51
    GenerateReservedVec         52
    GenerateReservedVec         53
    GenerateReservedVec         54
    GenerateReservedVec         55
    GenerateReservedVec         56
    GenerateReservedVec         57
    GenerateReservedVec         58
    GenerateReservedVec         59
    GenerateReservedVec         5A
    GenerateReservedVec         5B
    GenerateReservedVec         5C
    GenerateReservedVec         5D
    GenerateReservedVec         5E
    GenerateReservedVec         5F

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.section bss,bss
    ; register save area for exceptions, in typical movem order. the saved registers are d0-d7 and
    ; a0-a6, though space is reserved for a7.
    comm exception_regs, (4*16)

    ; when nonzero, bus errors are being caught
    comm catchberr, 1
    ; this is set to a nonzero value if a bus error occurred
    comm hasberr, 1
    ; copy of the most recent bus error stack frame
    comm berrframe, 14

