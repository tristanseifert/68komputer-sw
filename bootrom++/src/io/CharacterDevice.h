#pragma once

namespace io {
/**
 * Set of methods that a character based device (such as a serial port or printer) needs to
 * implement to be useable by the rest of the system.
 */
class CharacterDevice {
    public:
        /**
         * Determine whether the device is ready to transmit a character.
         */
        virtual bool writeAvailable() = 0;
        /**
         * Transmits a character through the device.
         *
         * @return 0 on success, or a negative error code.
         */
        virtual int write(char ch) = 0;

        /**
         * Determine whether a character is ready to read from the device.
         */
        virtual bool readAvailable() = 0;
        /**
         * Reads a character from the device.
         *
         * @return The read character, or a negative error code.
         */
        virtual int read() = 0;
};
}
