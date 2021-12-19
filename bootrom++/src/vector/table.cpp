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
    /* various error conditions */
    reinterpret_cast<void *>(&isr_bus_error), reinterpret_cast<void *>(&isr_address_error),
};

