#include "Xr68c681.h"
#include "io/Console.h"
#include "io/UnitTable.h"
#include "runtime/version.h"

#include <bootrom/Driver.h>

using namespace hw;

#define REG(x) (reinterpret_cast<volatile uint8_t *>(this->base) + static_cast<uint8_t>(Xr68C681::Reg::x))

uint32_t Xr68C681::gTicks{0};

/**
 * Define the values the DUART registers are reset to when the ROM initializes.
 */
const Xr68C681::RegInfo Xr68C681::gInitRegisters[kNumInitRegisters] = {
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
};

const driver_info_t Xr68C681::gDriverInfo {
    .magic=  'DRVR',
    .name = "XR68C681",
    .author = "Tristan Seifert",
    .description = "Built in XR68C681 driver",
    .version = "0.1",
};



/**
 * Initialize the DUART driver for a 68681 compatible UART at the given address. If the vector
 * specified is nonzero, it's enabled for interrupts as well.
 */
Xr68C681::Xr68C681(volatile void *base, const uint8_t vector) :
    base(reinterpret_cast<volatile uint8_t *>(base)), portA(this->base), portB(this->base + (8*2)) {
    this->resetRegisters();

    // set up IRQ vector
    if(vector) {
        this->irqSupported = true;
        *REG(IVR) = vector;

        // enable the timer irq
        *REG(IMR) = 0b00001000;
    }

    // register devices
    io::UnitTable::Register(io::UnitTable::Type::CharSerial, gDriverInfo, getPortA());
    io::UnitTable::Register(io::UnitTable::Type::CharSerial, gDriverInfo, getPortB());
}

/**
 * Resets the DUART and configures registers with default values.
 *
 * UART A is set up at 19.2kbaud and hardware flow control; UART B is set to 115.2kbaud and
 * hardware flow control. All LEDs are extinguished and the timer is set in counter mode, to
 * generate an interrupt at 100Hz.
 */
void Xr68C681::resetRegisters() {
    for(size_t i = 0; i < kNumInitRegisters; i++) {
        const auto &r = gInitRegisters[i];
        *(reinterpret_cast<volatile uint8_t *>(this->base) + static_cast<uint8_t>(r.index)) = r.value;
    }

    // start counter/timer
    (void) *REG(SCC);
}



/**
 * Write the given character to a port on the DUART.
 *
 * @param ch Character to write
 */
int Xr68C681::Port::write(char ch) {
    // ensure TXRDY
    while(!(*REG(SRA) & ChannelStatus::TxReady)) {}

    *REG(THRA) = ch;
    return 0;
}

/**
 * Checks whether the device is ready to accept another character to write.
 */
bool Xr68C681::Port::writeAvailable() {
    return !!(*REG(SRA) & ChannelStatus::TxReady);
}

/**
 * Reads a character from the port A receive FIFO.
 *
 * @return Oldest character in the receive FIFO.
 *
 * @remark Ensure a character is pending in the FIFO before trying to read one.
 */
int Xr68C681::Port::read() {
    // TODO: ensure a character is pending?
    return *REG(RHRA);
}

/**
 * Checks whether a character is waiting to be read from the port's receive FIFO.
 */
bool Xr68C681::Port::readAvailable() {
    return !!(*REG(SRA) & ChannelStatus::RxReady);
}

/**
 * Checks whether the transmit FIFO underran.
 */
bool Xr68C681::Port::txUnderrun() {
    return !!(*REG(SRA) & ChannelStatus::TxFifoEmpty);
}

/**
 * Checks whether the port's receive FIFO has been overrun
 */
bool Xr68C681::Port::rxOverrun() {
    return !!(*REG(SRA) & ChannelStatus::Overrun);
}

/**
 * Checks whether the port has encountered a framing error
 */
bool Xr68C681::Port::rxFramingError() {
    return !!(*REG(SRA) & ChannelStatus::FramingError);
}

/**
 * Checks whether the port received a break condition on the line
 */
bool Xr68C681::Port::rxBreak() {
    return !!(*REG(SRA) & ChannelStatus::Break);
}

/**
 * Clears any pending errors on the port.
 */
void Xr68C681::Port::clearErrors() {
    *REG(CRA) = MakeCommand(Command::ResetErrorStatus);
}

