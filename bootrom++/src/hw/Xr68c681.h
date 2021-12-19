#pragma once

#include <stddef.h>
#include <stdint.h>

namespace hw {
/**
 * Hardware driver for the XR68C681 DUART. It contains two serial ports, a 6 bit input port and a
 * 8 bit output port; as well as a programmable counter/timer capable of generating interrupts.
 *
 * This is a simple, fixed driver that handles the DUART on the CPU board. It registers its TTY
 * port with the console subsystem, and the timer is used as the system clock tick. The second
 * port is available for use inside the boot ROM.
 *
 * Additionally, it's assumed that the /CTS and /RTS hardware flow control lines are set up for
 * both serial ports; and that any remaining output ports are used for LED indicators. The input
 * ports are used for configuration DIP switches and for an interrupt line from the backplane I2C
 * bus.
 */
class Xr68C681 {
    public:
        static void Reset();

        static void PutCharA(char ch);
        static void PutCharB(char ch);

        Xr68C681() = delete;

        /**
         * Names of registers in the DUART
         *
         * The value of each register corresponds to its index multiplied by two, to simplify the
         * address calculation.
         */
        enum class Reg: uint8_t {
            /// RW - MR{1,2}A: Channel A mode register
            MRnA                        = (0x0 * 2),
            ///  W - Channel A clock select register
            CSRA                        = (0x1 * 2),
            ///  R - Channel A status register
            SRA                         = (0x1 * 2),
            ///  W - Channel A command register
            CRA                         = (0x2 * 2),
            ///  R - Masked interrupt status register
            MISR                        = (0x2 * 2),
            ///  R - Channel A receive holding register/FIFO
            RHRA                        = (0x3 * 2),
            ///  W - Channel A transmit holding register/FIFO
            THRA                        = (0x3 * 2),

            /// Sentinel value indicating end of register list
            End                         = (0x10 * 2),
        };

        /**
         * Bits inside a channel status register
         */
        enum ChannelStatus: uint8_t {
            /// Remote device sent a "break" signal
            Break                       = (1 << 7),
            /// A byte couldn't be correctly received
            FramingError                = (1 << 6),
            /// The parity bit did not match
            ParityError                 = (1 << 5),
            /// A byte was received before the FIFO was read out
            Overrun                     = (1 << 4),
            /// The TX FIFO is empty
            TxFifoEmpty                 = (1 << 3),
            /// The DUART is ready to transmit a byte
            TxReady                     = (1 << 2),
            /// The FIFO is full
            FifoFull                    = (1 << 1),
            /// The receiver is ready
            RxReady                     = (1 << 0),
        };

         /**
          * Base physical address of the DUART.
          *
          * It's assumed the DUART is a byte wide peripheral at the given address, and that we
          * need to increment the address by the width of the data bus to get to the next
          * register.
          */
        static const constexpr uintptr_t kBaseAddr{0x130001};
};
}
