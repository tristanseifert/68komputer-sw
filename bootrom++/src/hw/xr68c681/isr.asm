;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Interrupt handler for the XR68C681 DUART.
;
; Currently, this just checks whether the timer/counter 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.section code,code
    public isr_irq_xr68c681

; XXX: You _must_ update this if the value in Xr68c681.h changes
DUART_Base                      = $130001

; READ: IRQ status register
DUART_R_ISR               = $5*2
; READ: stop counter/timer
DUART_R_STC               = $F*2
; READ: Start counter/timer
DUART_R_SCC               = $E*2

isr_irq_xr68c681:
    movem       d0/a0, -(sp)

    lea         (DUART_Base), a0
    move.b      DUART_R_ISR(a0), d0

    ; handle timer elapsed, reset the timer
    btst        #3, d0
    beq.s       .noTimer

    ;move.b      DUART_R_STC(a0), d0
    ;move.b      DUART_R_SCC(a0), d0

    addq.l      #1, (_ZN2hw8Xr68C6816gTicksE).w

.noTimer:
    movem       (sp)+, d0/a0
    rte

