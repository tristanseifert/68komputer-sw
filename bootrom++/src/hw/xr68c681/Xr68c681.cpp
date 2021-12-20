#include "Xr68c681.h"
#include "io/Console.h"

using namespace hw;

#define REG(x) (reinterpret_cast<volatile uint8_t *>(Xr68C681::kBaseAddr) + static_cast<uint8_t>(Xr68C681::Reg::x))

uint32_t Xr68C681::gTicks{0};

/**
 * Define the values the DUART registers are reset to when the ROM initializes.
 */
const Xr68C681::RegInfo Xr68C681::gInitRegisters[Xr68C681::kNumInitRegisters] = {
    // mask all interrupts
    MakeRegister(Reg::IMR, 0b00000000),
    // disable TX and RX for each port
    MakeRegister(Reg::CRA, 0b00001010),
    MakeRegister(Reg::CRB, 0b00001010),

    // bit rate set #2, counter mode with CLK/16, no input change interrupts
    MakeRegister(Reg::ACR, 0b10110000),
    // configure the counter to tick at the right frequency: 230,400Hz / 4608 = 50Hz
    MakeRegister(Reg::CTU, 4608 >> 8),
    MakeRegister(Reg::CTL, 4608 & 0xFF),

    // port A: reset receiver and transmitter, error status, and break change interupts
    MakeRegister(Reg::CRA, 0b00100000),
    MakeRegister(Reg::CRA, 0b00110000),
    MakeRegister(Reg::CRA, 0b01000000),
    MakeRegister(Reg::CRA, 0b01010000),
    // port A: RxRDY IRQ, enable char FIFO errors, normal operation, 8N1 w/ HW flow control
    MakeRegister(Reg::CRA, 0b00010000), // reset MRnA pointer
    MakeRegister(Reg::MRnA, 0b10010011), // MR1B: RX RTS, RXRdy irq, char errors, no parity, 8 bits
    MakeRegister(Reg::MRnA, 0b00110111), // MR2B: normal, TX RTS/CTS, bit length = 1 clock
    // port A: 19,200 baud, active mode, enable TX and RX
    MakeRegister(Reg::CRA, 0b10010000), // clear RX BRG extend bit to 0
    MakeRegister(Reg::CRA, 0b10110000), // clear TX BRG extend bit to 0
    MakeRegister(Reg::CSRA, 0b11001100), // 19.2k
    MakeRegister(Reg::CRA, 0b11010000), // set active mode
    MakeRegister(Reg::CRA, 0b00000101), // enable RX/TX

    // port B: reset receiver and transmitter, error status, and break change interupts
    MakeRegister(Reg::CRB, 0b00100000),
    MakeRegister(Reg::CRB, 0b00110000),
    MakeRegister(Reg::CRB, 0b01000000),
    MakeRegister(Reg::CRB, 0b01010000),
    // port B: RxRDY IRQ, enable char FIFO errors, normal operation, 8N1 w/ HW flow control
    MakeRegister(Reg::CRB, 0b00010000), // reset MRnB pointer
    MakeRegister(Reg::MRnB, 0b10010011), // MR1B: RX RTS, RXRdy irq, char errors, no parity, 8 bits
    MakeRegister(Reg::MRnB, 0b00110111), // MR2B: normal, TX RTS/CTS, bit length = 1 clock
    // port B: 115,200 baud, active mode, enable TX and RX
    MakeRegister(Reg::CRB, 0b10000000), // set RX BRG extend bit to 1
    MakeRegister(Reg::CRB, 0b10100000), // set TX BRG extend bit to 1
    MakeRegister(Reg::CSRB, 0b10001000), // 115.2k
    MakeRegister(Reg::CRB, 0b11010000), // set active mode
    MakeRegister(Reg::CRB, 0b00000101), // enable RX/TX

    // configure output port; clear all LEDs except LED 4
    MakeRegister(Reg::OPCR, 0b00000000),
    MakeRegister(Reg::COPBC, 0b11111100),
    MakeRegister(Reg::SOPBC, 0b00000100),
    MakeRegister(Reg::CRB, 0b11010000), // set active mode
    MakeRegister(Reg::CRB, 0b00000101), // enable RX/TX

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

    // register port A as console output
    Console::SetOutHandler(Xr68C681::PutCharA);

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
