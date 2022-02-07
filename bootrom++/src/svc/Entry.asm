;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Trap service dispatcher
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.section code,code
    public isr_rom_svc_traphouse

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Entry point for the ROM services dispatcher (TRAP #15)
;
; Based on the function number in d0.w, it will invoke the correct handler function. This means the
; high word of d0 can be used for arguments. d0 and a0 are always clobbered, but other registers
; should be intact otherwise unless specified.
;
; @remarkTrap numbers always go in steps of 2 since the trap dispatch table is word wide. These
; trap handlers must terminate with `rte` _and_ set the status code/return value in d0.
isr_rom_svc_traphouse:
    ; ensure it's in bounds
    cmp.w       #(SvcJumpTableEnd-SvcJumpTable), d0
    bge.s       .invalidSvc

    ; then dispatch it
    bclr        #0, d0

    move.w      SvcJumpTable(pc, d0.w), d0
    jmp         SvcJumpTable(pc, d0.w)

; Service number is out of bounds
.invalidSvc:
    moveq       #-2, d0
    rte

SvcJumpTable:
    dc.w        SvcNoOp-SvcJumpTable
    dc.w        SvcTtyPuts-SvcJumpTable
    dc.w        SvcTtyPutch-SvcJumpTable
    dc.w        SvcTtyGetch-SvcJumpTable
    dc.w        SvcExitToMonitor-SvcJumpTable
    dc.w        SvcLz4Depack-SvcJumpTable
SvcJumpTableEnd:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Secretes the instructions for ending a service call.
;
; Currently, this is just an rte but making it a macro means we can extend this later much easier.

    macro SvcExit
    rte
    endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $00: No op
;
; Inputs:
;       None
;
; Outputs:
;       d0.w: 0
SvcNoOp:
    moveq       #0, d0
    SvcExit

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $02: Output zero terminated string on terminal
;
; Given a zero terminate (C style) string, output it to the currently active terminal.
;
; Inputs:
;       a0: Pointer to start of string to output (must be 0 terminated)
;
; Outputs:
;       d0.l: Number of characters written, or a negative error code
;
    global _ZN7Console3PutEPKc

SvcTtyPuts:
    movem.l     d1-d7/a1-a6, -(sp)

    link        fp, #-4
    move.l      a0, -(sp)
    bsr         _ZN7Console3PutEPKc
    unlk        fp

    ; TODO: chars written?
    moveq       #0, d0

    movem.l     (sp)+, d1-d7/a1-a6
    SvcExit

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $04: Output a character to the terminal.
;
; Inputs:
;       d1.b: Byte to write to the terminal
;
; Outputs:
;       None
;
    global      _putchar

SvcTtyPutch:
    movem.l     d1-d7/a0-a6, -(sp)

    link        fp, #-4
    move.l      d1, -(sp)
    bsr         _putchar
    unlk        fp

    ; TODO: error code?
    moveq       #0, d0

    movem.l     (sp)+, d1-d7/a0-a6
    SvcExit

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $06: Read a character from the terminal.
;
; Inputs:
;       None
;
; Outputs:
;       d0.l: The byte read from the terminal, or -1 if no characters are pending.
;
    global      _ZN7Console14GetCharWrapperEv

SvcTtyGetch:
    movem.l     d1-d7/a0-a6, -(sp)

    link        fp, #0
    bsr         _ZN7Console14GetCharWrapperEv
    unlk        fp

    movem.l     (sp)+, d1-d7/a0-a6
    SvcExit

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $08: Return to monitor
;
; Inputs:
;       None
;
; Outputs:
;       None
    global      ExecReturnHandler

SvcExitToMonitor:
    bra.w       ExecReturnHandler

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $0A: Decompress LZ4 frame
;
; Inputs:
;       a0: LZ4 frame to decompress
;       a1: Memory area to decompress data to
;
; Outputs:
;       None
    global      lz4_frame_depack

SvcLz4Depack:
    movem.l     d1-d5/d7/a2-a4, -(sp)

    bsr         lz4_frame_depack

    movem.l     (sp)+, d1-d5/d7/a2-a4
    SvcExit

