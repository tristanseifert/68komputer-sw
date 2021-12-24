#include "XModemReceiver.h"
#include "CharacterDevice.h"
#include "Console.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

using namespace io;

/**
 * Initializes the XMODEM receiver, including its state machine.
 *
 * @param remote Device to perform the transfer with
 */
XModemReceiver::XModemReceiver(CharacterDevice *remote) : remote(remote) {

}

/**
 * Starts the receive loop. The received data will be written into memory starting at the given
 * location.
 *
 * @param outBuf Location in memory to receive the data
 * @param progress Whether progress information is output on the console
 * @param outBufBytes Maximum number of bytes to receive, or 0 for no limit
 *
 * @return Number of bytes received or a negative error code
 */
int XModemReceiver::receive(void *outBuf, const bool progress, const size_t outBufBytes) {
    if(!outBuf) return ReceiveErrors::InvalidArguments;
    this->state = State::InitTransfer;

    /*
     * This is where the receive state machine is implemented. Many of these states are simple and
     * just transition to another one.
     */
    bool run{true};

    do {
        switch(this->state) {
            // Reset state machine and prepare for a new transfer.
            case State::InitTransfer:
                this->packetNum = 1;
                this->fileOffset = 0;
                this->retryCounter = kNumRetries;
                this->state = State::StartTransfer;
                break;
            // Request start of transfer.
            case State::StartTransfer:
                this->packetOffset = 0;
                this->state = State::ReceivePacket;

                this->packetTimeoutByte = static_cast<uint8_t>(Control::C);
                this->remote->write(static_cast<uint8_t>(Control::C));
                break;

            // Wait to read a complete 133 byte packet
            case State::ReceivePacket:
                /*
                 * TODO: handle timeouts here.
                 *
                 * This would go and send either the byte 'C', ACK, or NACK again depending on
                 * what state we came from and decrement the timeout retry counter.
                 */

                // read character into buffer
                if(this->remote->readAvailable()) {
                    const auto temp = this->remote->read();
                    if(temp < 0) return temp;
                    const auto byte = static_cast<uint8_t>(temp & 0xFF);

                    // special handling for the first byte so we can detect if the transfer is done
                    if(!this->packetOffset) {
                        // we're going to be reading a full frame
                        if(byte == static_cast<uint8_t>(Control::SOH)) {
                        } else if(byte == static_cast<uint8_t>(Control::EOT)) {
                            if(progress) {
                                Console::Put("\n* Received EOT\r\n");
                            }
                            this->state = State::AckEOT;
                            break;
                        }
                        else if(byte == static_cast<uint8_t>(Control::ETB)) {
                            if(progress) {
                                Console::Put("* Received ETB\r\n");
                            }
                            this->state = State::AckETB;
                            break;
                        }
                        // force cancelation of transfer
                        else if(byte == static_cast<uint8_t>(Control::CAN)) {
                            if(progress) {
                                Console::Put("\n! Transfer aborted by remote end\r\n");

                                this->state = State::AckCAN;
                                // XXX: this should be its own state transition
                            }
                        }
                        // invalid header value
                        else {
                            if(progress) {
                                Console::Print("! Invalid packet preamble: $%02x\r\n", byte);
                            }
                            return ReceiveErrors::InvalidPreamble;
                        }
                    }

                    // regardless, copy the byte into the buffer and check if we got a full packet
                    this->rxBuffer[this->packetOffset++] = byte;

                    if(this->packetOffset == sizeof(Packet)) {
                        this->state = State::ValidatePacket;
                    }
                }
                break;

            // Check the checksum on the packet
            case State::ValidatePacket: {
                const auto &packet = *reinterpret_cast<const Packet *>(rxBuffer);
                bool valid{true};

                // validate CRC
                uint16_t calculatedCrc{0};
                if(!CalcCrc(packet.payload, Packet::kPayloadSize, calculatedCrc)) {
                    // this should _never_ happen so it aborts the transfer
                    return ReceiveErrors::ChecksumFailed;
                }

                if(packet.crc != calculatedCrc) {
                    Console::Print("! Invalid CRC (calculated $%04x, actual $%04x)\r\n",
                            calculatedCrc, packet.crc);
                    valid = false;
                }
                else if(packet.preamble != static_cast<uint8_t>(Control::SOH)) {
                    Console::Print("! Invalid preamble $%02x\r\n", packet.preamble);
                    valid = false;
                }
                else if(packet.id != this->packetNum ||
                        packet.idComplement != (0xFF - this->packetNum)) {
                    Console::Print("! Invalid packet id (expected $%02x, actual $%02x)\r\n",
                            this->packetNum, packet.id);
                    valid = false;
                }

                if(!valid) {
                    if(progress) {
                        Console::Print("! Failed to verify packet $%02x\r\n", this->packetNum);
                    }
                    this->invalidPackets++;
                    this->state = State::RequestRetransmit;
                }
                // it's valid :)
                else {
                    this->state = State::AckPacket;
                    this->validPackets++;
                }
                break;
            }

            // The received packet was invalid and needs to be retransmitted.
            case State::RequestRetransmit:
                if(--this->retryCounter == 0) {
                    Console::Print("Exceeded retries receiving packet $%02x\r\n", this->packetNum);
                    return ReceiveErrors::ExceededRetries;
                }

                this->packetTimeoutByte = static_cast<uint8_t>(Control::NACK);
                this->state = State::ReceivePacket;

                this->remote->write(static_cast<uint8_t>(Control::NACK));
                break;

            // Packet was valid; copy it out, ack it and receive the next packet
            case State::AckPacket: {
                const auto &packet = *reinterpret_cast<const Packet *>(rxBuffer);

                // ensure sufficient space and copy out payload
                if(outBufBytes && outBufBytes < (this->fileOffset + Packet::kPayloadSize)) {
                    if(progress) {
                        Console::Print("! Insufficient buffer space (need %lu, have %lu)\r\n",
                                (this->fileOffset + Packet::kPayloadSize), outBufBytes);
                    }

                    // XXX: is this how we can abort the transfer?
                    this->remote->write(static_cast<uint8_t>(Control::CAN));
                    return ReceiveErrors::BufferOverflow;
                }

                // copy out payload data
                memcpy(reinterpret_cast<uint8_t *>(outBuf) + this->fileOffset, packet.payload,
                        Packet::kPayloadSize);
                this->fileOffset += Packet::kPayloadSize;

                if(progress) {
                    Console::Print("* %lu/%lu bytes\r", this->fileOffset, outBufBytes);
                }

                // reset the packet receive state for the next packet
                this->retryCounter = kNumRetries;
                this->packetOffset = 0;
                this->packetNum++;
                this->packetTimeoutByte = static_cast<uint8_t>(Control::ACK);
                this->state = State::ReceivePacket;

                this->remote->write(static_cast<uint8_t>(Control::ACK));
                break;
            }

            // Handle end of transfer
            case State::AckEOT:
                this->packetTimeoutByte = static_cast<uint8_t>(Control::ACK);
                this->state = State::ReceivePacket;
                run = false;

                this->remote->write(static_cast<uint8_t>(Control::ACK));
                break;

            // Handle end of transfer session
            case State::AckETB:
                this->packetTimeoutByte = static_cast<uint8_t>(Control::ACK);
                this->state = State::ReceivePacket;
                run = false;

                this->remote->write(static_cast<uint8_t>(Control::ACK));
                break;

            // Handle a transfer aborted by the remote
            case State::AckCAN:
                this->remote->write(static_cast<uint8_t>(Control::ACK));
                return ReceiveErrors::RemoteAborted;
        }


    } while(run);

    // the transfer is complete, so return how many bytes we received
    return this->fileOffset;
}




/**
 * Calculate the XMODEM style CRC16 over the given data block.
 *
 * @param data First byte of the block to process
 * @param size Number of bytes to process
 * @param outResult Variable to hold the calculated CRC
 *
 * @return Whether the CRC was calculated successfully
 */
bool XModemReceiver::CalcCrc(const uint8_t *data, const size_t size, uint16_t &outResult) {
    uint16_t crc{0};
    size_t count{size};
    uint8_t i{0};

    if(!data) return false;

    while(0 < count--) {
        crc = crc ^ (uint16_t) *data << 8;
        data++;
        i = 8;

        do {
            if(0x8000 & crc) {
                crc = crc << 1 ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        } while(0 < --i);
    }

    outResult = crc;

    return true;
}

