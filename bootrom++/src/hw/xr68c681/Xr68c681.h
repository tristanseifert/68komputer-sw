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
    private:
        Xr68C681() = delete;

    public:
        static void Reset();

        static void PutCharA(char ch);
        static bool TxEmptyA();
        static bool RxWaitingA();
        static bool RxOverrunA();
        static bool RxFramingErrorA();
        static bool RxBreakA();
        static void ClearErrorsA();
        static char GetCharA();

        static void PutCharB(char ch);
        static bool TxEmptyB();
        static bool RxWaitingB();
        static bool RxOverrunB();
        static bool RxFramingErrorB();
        static bool RxBreakB();
        static void ClearErrorsB();
        static char GetCharB();

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
            ///  R - Input port change register
            IPCR                        = (0x4 * 2),
            ///  W - Auxiliary control register
            ACR                         = (0x4 * 2),
            ///  R - Interrupt status register
            ISR                         = (0x5 * 2),
            ///  W - Interrupt mask register
            IMR                         = (0x5 * 2),
            /// RW - Counter/timer upper byte
            CTU                         = (0x6 * 2),
            /// RW - Counter/timer lower byte
            CTL                         = (0x7 * 2),

            /// RW - MR{1,2}B: Channel B mode register
            MRnB                        = (0x8 * 2),
            ///  W - Channel B clock select register
            CSRB                        = (0x9 * 2),
            ///  R - Channel B status register
            SRB                         = (0x9 * 2),
            ///  W - Channel B command register
            CRB                         = (0xA * 2),
            ///  R - Channel B receive holding register/FIFO
            RHRB                        = (0xB * 2),
            ///  W - Channel B transmit holding register/FIFO
            THRB                        = (0xB * 2),

            /// RW - Interrupt vector register
            IVR                         = (0xC * 2),
            ///  R - Input port
            IP                          = (0xD * 2),
            ///  W - Output port configuration register
            OPCR                        = (0xD * 2),
            ///  R - Start counter/timer command
            SCC                         = (0xE * 2),
            ///  W - Set output port bits command
            SOPBC                       = (0xE * 2),
            ///  R - Stop counter/timer command
            STC                         = (0xF * 2),
            ///  W - Clear output port bits command
            COPBC                       = (0xF * 2),

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
         *
         * @remark If this is changed, be sure to also update the ISR.
         */
        static const constexpr uintptr_t kBaseAddr{0x130001};

        /**
         * Interrupt vector reserved for use by the DUART. The ROM only makes use of this to update
         * the timer tick count.
         */
        static const constexpr uint8_t kIrqVector{0xF0};

    private:
        /// Command values to write to the command register
        enum class Command: uint8_t {
            /// No operation
            Null                        = (0b0000 << 4),
            /// Reset the MRn pointer to point to MR1
            ResetMRn                    = (0b0001 << 4),
            /// Resets the receiver and flushes its FIFO
            ResetReceiver               = (0b0010 << 4),
            /// Resets the transmitter
            ResetTransmitter            = (0b0011 << 4),
            /// Resets the error status bits
            ResetErrorStatus            = (0b0100 << 4),
            /// Clear the channel's break change interrupt status
            ResetBreakChangeIrq         = (0b0101 << 4),
            /// Start transmitting a break; wait for TXEMP first.
            StartBreak                  = (0b0110 << 4),
            /// Stop transmitting a break
            StopBreak                   = (0b0111 << 4),
            /// Sets the receiver baud rate set extend bit
            RxSetBaudRateExtend         = (0b1000 << 4),
            /// Clear the receiver baud rate set extend bit
            RxClearBaudRateExtend       = (0b1001 << 4),
            /// Sets the receiver baud rate set extend bit
            TxSetBaudRateExtend         = (0b1010 << 4),
            /// Clear the receiver baud rate set extend bit
            TxClearBaudRateExtend       = (0b1011 << 4),
            /// Enters standby mode
            EnterStandby                = (0b1100 << 4),
            /// Enter active mode
            SetActiveMode               = (0b1101 << 4),
        };

        /**
         * Builds a command register value with the given operation, but no change to the state of
         * the transmitter or receiver.
         *
         * @param opcode Operation to perform on the channel
         */
        static inline constexpr uint8_t MakeCommand(const Command opcode) {
            return static_cast<uint8_t>(opcode);
        }
        /**
         * Builds a command register value with a no-op operation, which enables or disables the
         * given channels.
         *
         * @param tx Whether the transmitter is enabled
         * @param rx Whether the receiver is enabled
         */
        static inline constexpr uint8_t MakeCommand(const bool tx, const bool rx) {
            return (tx ? 0b0100 : 0b1000) | (rx ? 0b0001 : 0b0010);
        }

    private:
        /// A single register write transaction
        struct RegInfo {
            Reg index;
            uint8_t value{0};
        };

        /// Creates a register initialization record
        static inline constexpr RegInfo MakeRegister(const Reg index, const uint8_t value) {
            return {
                .index = index,
                .value = value
            };
        }

        /// Number of registers to be written for initialization
        static const constexpr size_t kNumInitRegisters{34};
        /**
         * Register data to be written to the DUART at reset time.
         */
        static const RegInfo gInitRegisters[kNumInitRegisters];

        /**
         * Timer ticks since the last reset.
         *
         * Ticks occur at a rate of 50Hz, and this counter is incremented by one each time. Note
         * that this relies on the running code not disabling interrupts, or ticks will be lost.
         */
        static uint32_t gTicks;
};
}
