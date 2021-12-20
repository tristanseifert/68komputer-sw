#include "Xr68c681.h"
#include "io/Console.h"

using namespace hw;

#define REG(x) (reinterpret_cast<volatile uint8_t *>(Xr68C681::kBaseAddr) + static_cast<uint8_t>(Xr68C681::Reg::x))

uint32_t Xr68C681::gTicks{0};

/**
 * Define the values the DUART registers are reset to when the ROM initializes.
 */
const Xr68C681::RegInfo Xr68C681::gInitRegisters[Xr68C681::kNumInitRegisters] = {
    // enter active mode
    MakeRegister(Reg::CRA, MakeCommand(Command::SetActiveMode)),
    // mask all interrupts
    MakeRegister(Reg::IMR, 0b00000000),
    // disable TX and RX for each port
    MakeRegister(Reg::CRA, MakeCommand(false, false)),
    MakeRegister(Reg::CRB, MakeCommand(false, false)),

    // bit rate set #2, counter mode with CLK/16, no input change interrupts
    MakeRegister(Reg::ACR, 0b10110000),
    // configure the counter to tick at the right frequency: 230,400Hz / 4608 = 50Hz
    MakeRegister(Reg::CTU, 4608 >> 8),
    MakeRegister(Reg::CTL, 4608 & 0xFF),

    // port A: reset receiver and transmitter, error status, and break change interupts
    MakeRegister(Reg::CRA, MakeCommand(Command::ResetReceiver)),
    MakeRegister(Reg::CRA, MakeCommand(Command::ResetTransmitter)),
    MakeRegister(Reg::CRA, MakeCommand(Command::ResetErrorStatus)),
    MakeRegister(Reg::CRA, MakeCommand(Command::ResetBreakChangeIrq)),
    // port A: RxRDY IRQ, enable char FIFO errors, normal operation, 8N1 w/ HW flow control
    MakeRegister(Reg::CRA, MakeCommand(Command::ResetMRn)),
    MakeRegister(Reg::MRnA, 0b10010011), // MR1B: RX RTS, RXRdy irq, char errors, no parity, 8 bits
    MakeRegister(Reg::MRnA, 0b00110111), // MR2B: normal, TX RTS/CTS, bit length = 1 clock
    // port A: 19,200 baud, active mode, enable TX and RX
    MakeRegister(Reg::CRA, MakeCommand(Command::RxClearBaudRateExtend)),
    MakeRegister(Reg::CRA, MakeCommand(Command::TxClearBaudRateExtend)),
    MakeRegister(Reg::CSRA, 0b11001100), // 19.2k
    MakeRegister(Reg::CRA, MakeCommand(true, true)),

    // port B: reset receiver and transmitter, error status, and break change interupts
    MakeRegister(Reg::CRB, MakeCommand(Command::ResetReceiver)),
    MakeRegister(Reg::CRB, MakeCommand(Command::ResetTransmitter)),
    MakeRegister(Reg::CRB, MakeCommand(Command::ResetErrorStatus)),
    MakeRegister(Reg::CRB, MakeCommand(Command::ResetBreakChangeIrq)),
    // port B: RxRDY IRQ, enable char FIFO errors, normal operation, 8N1 w/ HW flow control
    MakeRegister(Reg::CRB, MakeCommand(Command::ResetMRn)),
    MakeRegister(Reg::MRnB, 0b10010011), // MR1B: RX RTS, RXRdy irq, char errors, no parity, 8 bits
    MakeRegister(Reg::MRnB, 0b00110111), // MR2B: normal, TX RTS/CTS, bit length = 1 clock
    // port B: 115,200 baud, active mode, enable TX and RX
    MakeRegister(Reg::CRB, MakeCommand(Command::RxSetBaudRateExtend)),
    MakeRegister(Reg::CRB, MakeCommand(Command::TxSetBaudRateExtend)),
    MakeRegister(Reg::CSRB, 0b10001000), // 115.2k
    MakeRegister(Reg::CRB, MakeCommand(true, true)),

    // configure output port; clear all LEDs except LED 4
    MakeRegister(Reg::OPCR, 0b00000000),
    MakeRegister(Reg::COPBC, 0b11111100),
    MakeRegister(Reg::SOPBC, 0b00000100),

    // configure interrupts
    MakeRegister(Reg::IVR, kIrqVector),
    MakeRegister(Reg::IMR, 0b00001000),
};


/**
 * Resets the DUART and configures registers with default values.
 *
 * UART A is set up at 19.2kbaud and hardware flow control; UART B is set to 115.2kbaud and
 * hardware flow control. All LEDs are extinguished and the timer is set in counter mode, to
 * generate an interrupt at 100Hz.
 */
void Xr68C681::Reset() {
    for(size_t i = 0; i < kNumInitRegisters; i++) {
        const auto &r = gInitRegisters[i];
        *(reinterpret_cast<volatile uint8_t *>(kBaseAddr) + static_cast<uint8_t>(r.index)) = r.value;
    }

    // register port A as console device
    Console::SetOutHandler(Xr68C681::PutCharA);
    Console::SetInPendingHandler(Xr68C681::RxWaitingA);
    Console::SetInHandler(Xr68C681::GetCharA);

    // start counter/timer
    (void) *REG(SCC);
}

/**
 * Write the given character to port A on the DUART.
 *
 * @param ch Character to write
 *
 * @remark This call will block until a character is available to write in the FIFO.
 */
void Xr68C681::PutCharA(char ch) {
    // wait for TXRDY
    while(!(*REG(SRA) & ChannelStatus::TxReady)) {}
    *REG(THRA) = ch;
}

/**
 * Checks whether the port A transmit FIFO is empty.
 *
 * @return Whether there's a character waiting in the transmit FIFO.
 */
bool Xr68C681::TxEmptyA() {
    return !!(*REG(SRA) & ChannelStatus::TxFifoEmpty);
}

/**
 * Checks whether a character is waiting to be read from the port A receive FIFO.
 *
 * @return Whether there's a character waiting in the FIFO.
 */
bool Xr68C681::RxWaitingA() {
    return !!(*REG(SRA) & ChannelStatus::RxReady);
}

/**
 * Checks whether the port A receive FIFO has been overrun
 *
 * @return FIFO overrun and a character was lost
 */
bool Xr68C681::RxOverrunA() {
    return !!(*REG(SRA) & ChannelStatus::Overrun);
}

/**
 * Checks whether the port A has received a framing error
 *
 * @return When set, a framing error occurred
 */
bool Xr68C681::RxFramingErrorA() {
    return !!(*REG(SRA) & ChannelStatus::FramingError);
}

/**
 * Checks whether the port A was received a break
 *
 * @return When set, a break was received
 */
bool Xr68C681::RxBreakA() {
    return !!(*REG(SRA) & ChannelStatus::Break);
}

/**
 * Clears any pending errors on port A.
 */
void Xr68C681::ClearErrorsA() {
    *REG(CRA) = 0b01000000;
}

/**
 * Reads a character from the port A receive FIFO.
 *
 * @return Oldest character in the receive FIFO.
 *
 * @remark Ensure a character is pending in the FIFO before trying to read one.
 */
char Xr68C681::GetCharA() {
    return *REG(RHRA);
}


/**
 * Write the given character to port B on the DUART.
 *
 * @param ch Character to write
 *
 * @remark This call will block until a character is available to write in the FIFO.
 */
void Xr68C681::PutCharB(char ch) {
    // wait for TXRDY
    while(!(*REG(SRB) & ChannelStatus::TxReady)) {}
    *REG(THRB) = ch;
}

/**
 * Checks whether the port B transmit FIFO is empty.
 *
 * @return Whether there's a character waiting in the transmit FIFO.
 */
bool Xr68C681::TxEmptyB() {
    return !!(*REG(SRB) & ChannelStatus::TxFifoEmpty);
}

/**
 * Checks whether a character is waiting to be read from the port B receive FIFO.
 *
 * @return Whether there's a character waiting in the FIFO.
 */
bool Xr68C681::RxWaitingB() {
    return !!(*REG(SRB) & ChannelStatus::RxReady);
}

/**
 * Checks whether the port B receive FIFO has been overrun
 *
 * @return FIFO overrun and a character was lost
 */
bool Xr68C681::RxOverrunB() {
    return !!(*REG(SRB) & ChannelStatus::Overrun);
}

/**
 * Checks whether the port B has received a framing error
 *
 * @return When set, a framing error occurred
 */
bool Xr68C681::RxFramingErrorB() {
    return !!(*REG(SRB) & ChannelStatus::FramingError);
}

/**
 * Checks whether the port B was received a break
 *
 * @return When set, a break was received
 */
bool Xr68C681::RxBreakB() {
    return !!(*REG(SRB) & ChannelStatus::Break);
}

/**
 * Clears any pending errors on port B.
 */
void Xr68C681::ClearErrorsB() {
    *REG(CRB) = 0b01000000;
}

/**
 * Reads a character from the port B receive FIFO.
 *
 * @return Oldest character in the receive FIFO.
 *
 * @remark Ensure a character is pending in the FIFO before trying to read one.
 */
char Xr68C681::GetCharB() {
    return *REG(RHRB);
}
