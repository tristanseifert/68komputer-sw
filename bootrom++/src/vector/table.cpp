#include "table.h"
#include "handlers.h"

/// Address of this symbol is the top of the stack
extern "C" void *_stack_ptr;

/*
 * Vector table; this is stored directly at the start of the ROM in the vector table section.
 */
const __attribute__((section(".vectors"))) void *Vectors::gVectorTable[Vectors::kNumVectors] = {
    /* reset stack and pc */
    (void *) &_stack_ptr, reinterpret_cast<void *>(&romentry),
    /* error handlers */
    &gTrampolines[TrampolineIndex::BusError], &gTrampolines[TrampolineIndex::AddressError],
    &gTrampolines[TrampolineIndex::IllegalInstruction], &gTrampolines[TrampolineIndex::ZeroDivide],
    &gTrampolines[TrampolineIndex::Chk], &gTrampolines[TrampolineIndex::TrapOverflow],
    &gTrampolines[TrampolineIndex::PrivilegeViolation], &gTrampolines[TrampolineIndex::Trace],
    &gTrampolines[TrampolineIndex::Line1010], &gTrampolines[TrampolineIndex::Line1111],

    /* two reserved vectors */
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),

    /* vector $E is "format error" for 68010 */
    reinterpret_cast<void *>(&isr_reserved),

    /* vector $F is uninitialized interrupt vector */
    &gTrampolines[TrampolineIndex::UninitializedIrq],

    /* 8 more reserved/unassigned vectors */
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),

    /* spurious irq and autovectors for levels 1-3 */
    &gTrampolines[TrampolineIndex::SpuriousIrq], &gTrampolines[TrampolineIndex::Irq1AutoVector],
    &gTrampolines[TrampolineIndex::Irq2AutoVector], &gTrampolines[TrampolineIndex::Irq3AutoVector],
    /* levels 4-6 are CPU board peripherals, and they are _not_ autovectored */
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved),
    /* we always catch the NMI to enter into the monitor */
    reinterpret_cast<void *>(&isr_reserved),
    /* all TRAP vectors - [0,14] can be overridden, 15 is used for ROM services */
    &gTrampolines[TrampolineIndex::Trap0], &gTrampolines[TrampolineIndex::Trap1],
    &gTrampolines[TrampolineIndex::Trap2], &gTrampolines[TrampolineIndex::Trap3],
    &gTrampolines[TrampolineIndex::Trap4], &gTrampolines[TrampolineIndex::Trap5],
    &gTrampolines[TrampolineIndex::Trap6], &gTrampolines[TrampolineIndex::Trap7],
    &gTrampolines[TrampolineIndex::Trap8], &gTrampolines[TrampolineIndex::Trap9],
    &gTrampolines[TrampolineIndex::Trap10], &gTrampolines[TrampolineIndex::Trap11],
    &gTrampolines[TrampolineIndex::Trap12], &gTrampolines[TrampolineIndex::Trap13],
    &gTrampolines[TrampolineIndex::Trap14],

    reinterpret_cast<void *>(&isr_rom_svc_trap),

    /* vectors $30 - $3F are unassigned/reserved */
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),

    /* any remaining vectors go towards user interrupts */
    &gTrampolines[TrampolineIndex::UserIrqStart + 0],
    &gTrampolines[TrampolineIndex::UserIrqStart + 1],
    &gTrampolines[TrampolineIndex::UserIrqStart + 2],
    &gTrampolines[TrampolineIndex::UserIrqStart + 3],
    &gTrampolines[TrampolineIndex::UserIrqStart + 4],
    &gTrampolines[TrampolineIndex::UserIrqStart + 5],
    &gTrampolines[TrampolineIndex::UserIrqStart + 6],
    &gTrampolines[TrampolineIndex::UserIrqStart + 7],
    &gTrampolines[TrampolineIndex::UserIrqStart + 8],
    &gTrampolines[TrampolineIndex::UserIrqStart + 9],
    &gTrampolines[TrampolineIndex::UserIrqStart + 10],
    &gTrampolines[TrampolineIndex::UserIrqStart + 11],
    &gTrampolines[TrampolineIndex::UserIrqStart + 12],
    &gTrampolines[TrampolineIndex::UserIrqStart + 13],
    &gTrampolines[TrampolineIndex::UserIrqStart + 14],
    &gTrampolines[TrampolineIndex::UserIrqStart + 15],
    &gTrampolines[TrampolineIndex::UserIrqStart + 16],
    &gTrampolines[TrampolineIndex::UserIrqStart + 17],
    &gTrampolines[TrampolineIndex::UserIrqStart + 18],
    &gTrampolines[TrampolineIndex::UserIrqStart + 19],
    &gTrampolines[TrampolineIndex::UserIrqStart + 20],
    &gTrampolines[TrampolineIndex::UserIrqStart + 21],
    &gTrampolines[TrampolineIndex::UserIrqStart + 22],
    &gTrampolines[TrampolineIndex::UserIrqStart + 23],
    &gTrampolines[TrampolineIndex::UserIrqStart + 24],
    &gTrampolines[TrampolineIndex::UserIrqStart + 25],
    &gTrampolines[TrampolineIndex::UserIrqStart + 26],
    &gTrampolines[TrampolineIndex::UserIrqStart + 27],
    &gTrampolines[TrampolineIndex::UserIrqStart + 28],
    &gTrampolines[TrampolineIndex::UserIrqStart + 29],
    &gTrampolines[TrampolineIndex::UserIrqStart + 30],
    &gTrampolines[TrampolineIndex::UserIrqStart + 31],

    /* any remaining vectors are reserved */
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),

    /* the last 16 vectors are used for hw interrupts by the boot ROM */
    reinterpret_cast<void *>(&isr_irq_xr68c681),

    reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
    reinterpret_cast<void *>(&isr_reserved), reinterpret_cast<void *>(&isr_reserved),
};

/**
 * Storage area for interrupt handler trampolines.
 *
 * On reset, these are initialized to point to our implementations for them.
 */
Vectors::TrampolineEntry Vectors::gTrampolines[Vectors::kNumTrampolines];

/**
 * List of interrupt handlers and their associated functions and handler settings. These are copied
 * into the trampoline area on startup.
 */
const Vectors::Handler Vectors::gDefaultHandlers[Vectors::kNumDefaultHandlers] = {
    // exceptions
    MakeHandler(TrampolineIndex::BusError, isr_bus_error),
    MakeHandler(TrampolineIndex::AddressError, isr_address_error),
    MakeHandler(TrampolineIndex::IllegalInstruction, isr_illegal_instruction),
    MakeHandler(TrampolineIndex::ZeroDivide, isr_zero_divide),
    MakeHandler(TrampolineIndex::Chk, isr_chk),
    MakeHandler(TrampolineIndex::TrapOverflow, isr_trapv),
    MakeHandler(TrampolineIndex::PrivilegeViolation, isr_privilege_violation),
    MakeHandler(TrampolineIndex::Trace, isr_trace),
    MakeHandler(TrampolineIndex::Line1010, isr_line1010),
    MakeHandler(TrampolineIndex::Line1111, isr_line1111),
    // interrupts
    MakeHandler(TrampolineIndex::UninitializedIrq, isr_irq_uninitialized),
    MakeHandler(TrampolineIndex::SpuriousIrq, isr_irq_spurious),
    MakeHandler(TrampolineIndex::Irq1AutoVector, isr_irq_unhandled_avec1),
    MakeHandler(TrampolineIndex::Irq2AutoVector, isr_irq_unhandled_avec2),
    MakeHandler(TrampolineIndex::Irq3AutoVector, isr_irq_unhandled_avec3),
    // unhandled traps
    MakeHandler(TrampolineIndex::Trap0, isr_reserved_20),
    MakeHandler(TrampolineIndex::Trap1, isr_reserved_21),
    MakeHandler(TrampolineIndex::Trap2, isr_reserved_22),
    MakeHandler(TrampolineIndex::Trap3, isr_reserved_23),
    MakeHandler(TrampolineIndex::Trap4, isr_reserved_24),
    MakeHandler(TrampolineIndex::Trap5, isr_reserved_25),
    MakeHandler(TrampolineIndex::Trap6, isr_reserved_26),
    MakeHandler(TrampolineIndex::Trap7, isr_reserved_27),
    MakeHandler(TrampolineIndex::Trap8, isr_reserved_28),
    MakeHandler(TrampolineIndex::Trap9, isr_reserved_29),
    MakeHandler(TrampolineIndex::Trap10, isr_reserved_2A),
    MakeHandler(TrampolineIndex::Trap11, isr_reserved_2B),
    MakeHandler(TrampolineIndex::Trap12, isr_reserved_2C),
    MakeHandler(TrampolineIndex::Trap13, isr_reserved_2D),
    MakeHandler(TrampolineIndex::Trap14, isr_reserved_2E),

    // user irq's
    MakeHandler(TrampolineIndex::UserIrqStart + 0x00, isr_reserved_40),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x01, isr_reserved_41),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x02, isr_reserved_42),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x03, isr_reserved_43),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x04, isr_reserved_44),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x05, isr_reserved_45),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x06, isr_reserved_46),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x07, isr_reserved_47),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x08, isr_reserved_48),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x09, isr_reserved_49),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x0A, isr_reserved_4A),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x0B, isr_reserved_4B),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x0C, isr_reserved_4C),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x0D, isr_reserved_4D),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x0E, isr_reserved_4E),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x0F, isr_reserved_4F),

    MakeHandler(TrampolineIndex::UserIrqStart + 0x10, isr_reserved_50),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x11, isr_reserved_51),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x12, isr_reserved_52),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x13, isr_reserved_53),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x14, isr_reserved_55),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x15, isr_reserved_55),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x16, isr_reserved_56),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x17, isr_reserved_57),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x18, isr_reserved_58),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x19, isr_reserved_59),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x1A, isr_reserved_5A),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x1B, isr_reserved_5B),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x1C, isr_reserved_5C),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x1D, isr_reserved_5D),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x1E, isr_reserved_5E),
    MakeHandler(TrampolineIndex::UserIrqStart + 0x1F, isr_reserved_5F),
};


/**
 * Initializes the interrupt vectors to the defaults implemented by the boot ROM.
 */
void Vectors::InitTrampolines() {
    for(size_t i = 0; i < kNumDefaultHandlers; i++) {
        const auto &entry = gDefaultHandlers[i];

        if(entry.rte) {
            gTrampolines[entry.index].opcode = 0x4E73; // RTE
            gTrampolines[entry.index].addr = 0;
        } else if(entry.handle) {
            gTrampolines[entry.index].opcode = 0x4EF9; // JMP
            gTrampolines[entry.index].addr = reinterpret_cast<uint32_t>(entry.handler);
        } else {
            // XXX: this shouldn't happen!
        }
    }
}
