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
    dbf         d1, .loop

    ; set the flag indicating we've warm booted
    move.w      #'YE', (bootflag).w

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.common:
    ; copy .data to RAM
    lea         __data_rom_start, a0
    lea         __data_ram_start, a1
    move.l      #__data_rom_dwords, d0
    ; -1 = there's no data to copy
    cmp.w       #-1, d0
    beq.s       .nodata

.copydata:
    move.l      (a0)+, (a1)+
    dbf         d0, .copydata

.nodata:
    ; clear .bss
    lea         __bss_start, a0
    move.l      #__bss_dwords, d0
    ; -1 = there's no bss to clear
    cmp.w       #-1, d0
    beq.s       .nobss

    moveq       #0, d1

.clearbss:
    move.l      d1, (a0)+
    dbf         d0, .clearbss

.nobss:
    ; set up drivers/hardware
    bsr         hw_init

    ; run main function; it shouldn't ever return, but if it does, just reset
    bsr         bootrom_start
    bra         .warm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.section bss,bss
    ; used to differentiate a warm vs cold boot
    comm bootflag, 2
    ; indicates why were rebooted: 0 = cold boot, 1 = warm reset
    comm resetreason, 1
