#include "Xr68c681.h"

using namespace hw;

/**
 * Resets the DUART and configures registers with default values.
 *
 * UART A is set up at 19.2kbaud and hardware flow control; UART B is set to 115.2kbaud and
 * hardware flow control. All LEDs are extinguished and the system timer is configured.
 */
void Xr68C681::Reset() {

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
    while(!(*GetReg(Reg::SRA) & ChannelStatus::TxReady)) {}
    *GetReg(Reg::THRA) = ch;
}

