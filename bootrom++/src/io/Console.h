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

    private:
        /**
         * Function to invoke for printing a character to the console.
         */
        static void (*gPutChar)(const char);
};
