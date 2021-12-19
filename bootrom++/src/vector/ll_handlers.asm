;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Low level ISR handlers
;
; Implementations of the assembly interrupt and exception handlers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.section code,code
    public isr_bus_error
    public isr_address_error


; CPU encountered a bus error during the most recent access
isr_bus_error:
    rte

; A word or longword access to an odd address was attempted.
isr_address_error:
    rte
