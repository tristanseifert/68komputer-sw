#pragma once

#include <stddef.h>
#include <stdint.h>

class Vectors {
    public:
        static void InitTrampolines();

    private:
        /// Wrapper defining a trampoline entry
        union TrampolineEntry {
            uint8_t data[8];
            struct {
                uint16_t opcode;
                uint32_t addr;
            };
        } __attribute__((packed));

        /// Indices into the trampoline array for exceptions
        enum TrampolineIndex: uint8_t {
            BusError                    = 0,
            AddressError                = 1,
            IllegalInstruction          = 2,
            ZeroDivide                  = 3,
            Chk                         = 4,
            TrapOverflow                = 5,
            PrivilegeViolation          = 6,
            Trace                       = 7,
            Line1010                    = 8,
            Line1111                    = 9,
            UninitializedIrq            = 10,
            SpuriousIrq                 = 11,
            Irq1AutoVector              = 12,
            Irq2AutoVector              = 13,
            Irq3AutoVector              = 14,
            Trap0                       = 15,
            Trap1                       = 16,
            Trap2                       = 17,
            Trap3                       = 18,
            Trap4                       = 19,
            Trap5                       = 20,
            Trap6                       = 21,
            Trap7                       = 22,
            Trap8                       = 23,
            Trap9                       = 24,
            Trap10                      = 25,
            Trap11                      = 26,
            Trap12                      = 27,
            Trap13                      = 28,
            Trap14                      = 29,
            // This is not yet used for anything
            Reserved                    = 30,
            /// First user interrupt vector
            UserIrqStart                = 31,
        };

        /**
         * Defines a handler for a particular exception or interrupt.
         */
        struct Handler {
            /// trampoline index for this handler
            TrampolineIndex index;

            /// if set, the handler will just return immediately
            uint8_t rte         : 1{1};
            /// if set, we'll jump to the specified address
            uint8_t handle      : 1{0};

            union {
                /// if it's to be handled, the method to do so
                void (*handler)(){nullptr};
            };
        };

        /**
         * Create an ISR handler that invokes the given function. It must be written as an ISR
         * handler stub, e.g. backing up and restoring registers and existing with `rte`.
         */
        static constexpr Handler MakeHandler(const uint8_t index, void (*isr)()) {
            return {
                .index = static_cast<TrampolineIndex>(index),
                .rte = 0, .handle = 1,
                .handler = isr
            };
        }

        /**
         * Create an ISR handler that returns immediately.
         */
        static constexpr Handler MakeHandler(const uint8_t index) {
            return {
                .index = static_cast<TrampolineIndex>(index),
                .rte = 1, .handle = 0,
                .handler = nullptr,
            };
        }

    private:
        /// Total number of vectors to reserve space for
        constexpr static const size_t kNumVectors{256};
        /// Vector table (placed in ROM)
        static const void *gVectorTable[kNumVectors];

        /**
         * Total number of trampoline slots to allocate
         *
         * A minimum of 16 trampoline slots are needed for various CPU exceptions; any remaining
         * slots are used for the user interrupt vectors, starting at vector $40.
         */
        constexpr static const size_t kNumTrampolines{64};
        /**
         * Trampolines, reserved in RAM, for various overrideable interrupts.
         *
         * Inside each of these trampolines are 8 bytes of space; sufficient for a jump instruction
         * and an absolute address.
         */
        static TrampolineEntry gTrampolines[kNumTrampolines];

        /// Total number of default exception handlers to install
        constexpr static const size_t kNumDefaultHandlers{62};
        /**
         * List of all default exception handlers to be installed.
         *
         * Each entry configures the exception to either return immediately, or to jump to a
         * particular handler function.
         */
        static const Handler gDefaultHandlers[kNumDefaultHandlers];
};
