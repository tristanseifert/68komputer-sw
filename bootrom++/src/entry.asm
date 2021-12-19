;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Entry point for the Boot ROM
;
; This just determines whether we come from a cold or warm reset, sets up the memory (zeroing RAM
; on a cold reset, and just reinitializing the monitor data structures on a warm reset) and doing
; some basic hw initialization.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.section code,code
    public romentry

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
romentry:
    move        #$2700, sr

    ; determine if this is a warm or cold reset
    cmp.w       #'YE', (bootflag).w
    bne.s       .cold

.warm:
    move.b      #1, (resetreason).w
    bra.s       .common

.cold:
    ; reset external devices
    reset

    ; clear all RAM
    lea         ($FFF00000), a0
    moveq       #0, d0

    move.w      #((1024*1024)/32)-1, d1

.loop:
    rept 8
    move.l      d0, (a0)+
    endr
;    dbf         d1, .loop

    ; set the flag indicating we've warm booted
    move.w      #'YE', (bootflag).w

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.common:
    ; set up C runtime (copy .data)
    lea         shit, a0
    bsr.w       Console_Puts

    ; set up drivers/hardware
    jsr         hw_init

    ; run main function; it shouldn't ever return, but if it does, just reset
    jsr         bootrom_start
    bra         .warm


shit:
    dc.b        "Hello world!\r\n", 0
    even

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DUART_Base                      = $130001

; Vector number assigned for the DUART IRQ
DUART_ISR_Vector                = $F0

; WRITE: channel A mode register
DUART_W_MRnA              = $0*2
; WRITE: channel A clock select
DUART_W_CSRA              = $1*2
; READ: channel A status register
DUART_R_SRA               = $1*2
; WRITE: channel A command register
DUART_W_CRA               = $2*2
; READ: masked IRQ status
DUART_R_MISR              = $2*2
; WRITE: channel A TX holding register
DUART_W_THRA              = $3*2
; READ: channel A RX holding register
DUART_R_RHRA              = $3*2

; WRITE: AUX control register
DUART_W_ACR               = $4*2
; READ: Input port change register
DUART_R_ACR               = $4*2

; WRITE: IRQ mask register
DUART_W_IMR               = $5*2
; READ: IRQ status register
DUART_R_ISR               = $5*2

; WRITE: Counter/Timer upper byte
DUART_W_CTU               = $6*2
; READ: Counter/Timer upper byte
DUART_R_CTU               = DUART_W_CTU

; WRITE: Counter/Timer lower byte
DUART_W_CTL               = $7*2
; READ: Counter/Timer lower byte
DUART_R_CTL               = DUART_W_CTL

; WRITE: channel B mode register
DUART_W_MRnB              = $8*2
; WRITE: channel B clock select
DUART_W_CSRB              = $9*2
; READ: channel B status register
DUART_R_SRB               = $9*2
; WRITE: channel B command register
DUART_W_CRB               = $A*2
; WRITE: channel B TX holding register
DUART_W_THRB              = $B*2
; READ: channel B RX holding register
DUART_R_RHRB              = $B*2

; WRITE: IRQ vector register
DUART_W_IVR               = $C*2
; READ: IRQ vector register
DUART_R_IVR               = DUART_W_IVR


; WRITE: output port configuration
DUART_W_OPCR              = $D*2
; READ: input port
DUART_R_IP                = $D*2

; WRITE: set output port register bits
DUART_W_SOPBC             = $E*2
; READ: Start counter/timer
DUART_R_SCC               = $E*2

; WRITE: clear output port register bits
DUART_W_COPBC             = $F*2
; READ: stop counter/timer
DUART_R_STC               = $F*2

Console_Puts:
    move.b      (a0)+, d0
    beq.s       .done

.wait
    ; read the status register, check TXRDY bit (2), loop while clear
    btst    #2, (DUART_Base+DUART_R_SRA)
    beq     .wait

    ; write character to DUART
    move.b  d0, (DUART_Base+DUART_W_THRA)


    bra.s       Console_Puts

.done:
    rts

.section bss,bss
    comm bootflag, 2
    ; indicates why were rebooted: 0 = cold boot, 1 = warm reset
    comm resetreason, 1
