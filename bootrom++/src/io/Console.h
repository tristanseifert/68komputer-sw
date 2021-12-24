#pragma once

#include "CharacterDevice.h"

#include <stdarg.h>
#include <stddef.h>

class Console {
    public:
        static void Init();

        /**
         * Writes a character to all registered console output devices.
         *
         * @param ch Single character to write to the console
         */
        static inline void Put(const char ch) {
            gOutputDevice->write(ch);
        }

        static void Put(const char *);
        static int Print(const char* format, ...) __attribute__ ((format (printf, 1, 2)));

        /**
         * Check if there are characters pending to be read from the console.
         *
         * @return Whether one or more characters are pending to be read
         */
        static bool GetCharPending() {
            return gOutputDevice->readAvailable();
        }

        /**
         * Reads a character from the console.
         *
         * @return Character from the top of the receive queue
         *
         * @remark You should ensure there are pending characters; otherwise, the return value is
         *         undefined.
         */
        static char GetChar() {
            return gOutputDevice->read();
        }

        /**
         * Get the character device being used for console IO.
         *
         * @return Current character device
         */
        static auto GetDevice() {
            return gOutputDevice;
        }

        /**
         * Sets a character device to be used for all console IO.
         *
         * @param newDevice New character device
         *
         * @return Current character device
         */
        static auto SetDevice(io::CharacterDevice *newDevice) {
            auto old = gOutputDevice;
            gOutputDevice = newDevice;
            return old;
        }

    private:
        /**
         * Character device to be used for console output
         */
        static io::CharacterDevice *gOutputDevice;
};
