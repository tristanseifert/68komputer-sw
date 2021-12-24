#pragma once

#include <stddef.h>
#include <stdint.h>

namespace io {
class CharacterDevice;

/**
 * Handles receiving binary data from a remote host via XMODEM.
 *
 * Received data is read from a CharacterDevice, and written to an arbitrary location in memory.
 *
 * @remark This requires the sender receive XMODEM with CRC support. We do not support the 'old'
 *         XMODEM protocol with other checksums.
 *
 * TODO: There is no timeout handling of any sort
 */
class XModemReceiver {
    public:
        /// Error codes returned by the receive code
        enum ReceiveErrors: int {
            /// Insufficient space in receive buffer
            BufferOverflow              = -2,
            /// Exceeded allowable retries on packet receive
            ExceededRetries             = -3,
            /// Transfer was aborted by the remote (`CAN` received)
            RemoteAborted               = -4,
            /// Packet preamble was not one of the allowed values
            InvalidPreamble             = -5,
            /// Invalid arguments
            InvalidArguments            = -6,
            /// A received packet's checksum could not be calculated
            ChecksumFailed              = -7,
        };

    public:
        XModemReceiver(CharacterDevice *remote);

        int receive(void *outBuf, const bool progress = false, const size_t outBufBytes = 0);

    private:
        /**
         * States for the receive state machine
         */
        enum class State: uint8_t {
            /// Set up for a new file transfer
            InitTransfer,
            /// Send the initial "C" character to start the transfer in CRC mode.
            StartTransfer,
            /// Wait to receive a block
            ReceivePacket,
            /// Validates a packet (check the CRC)
            ValidatePacket,
            /// Sends a NACK to request retransmission of the last received packet
            RequestRetransmit,
            /// Sends an ACK to request transmission of the next packet.
            AckPacket,
            /// Sends an AKC for the "end of transfer" that signals file completion
            AckEOT,
            /// Sends an ACK for the "end transfer block" which means we're done.
            AckETB,
            /// Sends an ACK in response to a remote requested cancelation
            AckCAN,
        };

        /**
         * Control characters used by the XMODEM protocol
         */
        enum class Control: uint8_t {
            SOH                         = 0x01,
            EOT                         = 0x04,
            ACK                         = 0x06,
            NACK                        = 0x15,
            ETB                         = 0x17,
            CAN                         = 0x18,
            // for indicating CRC-16 support
            C                           = 0x43,
        };

        /**
         * Format of an XMODEM packet on the wire.
         */
        struct Packet {
            /// Size of the packet payload, fixed at 128 bytes
            constexpr static const size_t kPayloadSize{128};

            uint8_t preamble;
            /// packet ID
            uint8_t id;
            /// logical complement of `id`
            uint8_t idComplement;
            /// packet payload
            uint8_t payload[kPayloadSize];
            /// CRC over the data
            uint16_t crc{0};
        } __attribute__((packed));

    private:
        static bool CalcCrc(const uint8_t *data, const size_t size, uint16_t &outResult);

    private:
        /// Default value for the retry counter
        constexpr static const uint8_t kNumRetries{5};

        /// Device connecting to the remote end
        CharacterDevice *remote;

        /// Write offset into the output buffer
        size_t fileOffset;

        /**
         * Retry counter; this defines the number of times we try to resend a packet if we time out
         * waiting to get a response.
         */
        uint8_t retryCounter;
        /// Next packet number
        uint8_t packetNum;
        /// Number of bytes of the current packet received so far
        uint8_t packetOffset;
        /// Current state of the receiver state machine
        State state;
        /// If we time out receiving a packet, the byte to send to the remote to resume
        uint8_t packetTimeoutByte;

        /// counter of valid packets received
        uint16_t validPackets{0};
        /// number of packets that failed verification for some reason
        uint16_t invalidPackets{0};

        /// Packet receive buffer
        uint8_t rxBuffer[sizeof(Packet)];
};
}
