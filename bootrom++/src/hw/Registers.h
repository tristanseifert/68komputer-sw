#pragma once

#include <stdint.h>

/**
 * Defines the structure of a set of registers saved in memory.
 *
 * It follows the layout of how the `MOVEM` instruction would write them; but not how it would if
 * it were saving to the stack.
 */
struct CpuRegisters {
    /// Data registers
    uint32_t d[8];
    /// Address registers, where a7 = sp
    uint32_t a[8];

    /// if available, the program counter value
    uint32_t pc;
    /// status register at call time
    uint16_t sr;
} __attribute__((packed, aligned(4)));

