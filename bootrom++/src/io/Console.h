#pragma once

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
            gPutChar(ch);
        }

        static void Put(const char *);
        static int Print(const char* format, ...) __attribute__ ((format (printf, 1, 2)));

        /**
         * Check if there are characters pending to be read from the console.
         *
         * @return Whether one or more characters are pending to be read
         */
        static bool GetCharPending() {
            return gCharPending();
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
            return gGetChar();
        }

        /**
         * Sets the character output handler for the console.
         *
         * @param newHandler New handler function called to print a character
         *
         * @return Current handler function
         */
        static auto SetOutHandler(void (*newHandler)(const char)) {
            auto old = gPutChar;
            gPutChar = newHandler;
            return old;
        }

        /**
         * Sets the character pending handler for the console.
         *
         * @param newHandler New handler function called to check if characters are pending
         *
         * @return Current handler function
         */
        static auto SetInPendingHandler(bool (*newHandler)()) {
            auto old = gCharPending;
            gCharPending = newHandler;
            return old;
        }

        /**
         * Sets the character input handler for the console.
         *
         * @param newHandler New handler function called to read a character
         *
         * @return Current handler function
         */
        static auto SetInHandler(char (*newHandler)()) {
            auto old = gGetChar;
            gGetChar = newHandler;
            return old;
        }

    private:
        /**
         * Function to invoke for printing a character to the console.
         */
        static void (*gPutChar)(const char);

        /**
         * Function to invoke to check if a character can be read.
         */
        static bool (*gCharPending)();

        /**
         * Function to pop a character off the receive queue.
         */
        static char (*gGetChar)();
};
