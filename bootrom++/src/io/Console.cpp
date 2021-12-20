#include "Console.h"
#include "hw/Xr68c681.h"

#include <printf.h>

/**
 * Thunk through the bare printf output to the console.
 */
void _putchar(char ch) {
    Console::Put(ch);
}



/**
 * Initializes the console system.
 */
void Console::Init() {
    // TODO: do stuff
}



/**
 * Writes a character to all registered console output devices.
 *
 * @param ch Single character to write to the console
 */
void Console::Put(const char ch) {
    hw::Xr68C681::PutCharA(ch);
}

/**
 * Writes a string to the console.
 *
 * @param str A NULL-terminated string to print to the console as-is
 */
void Console::Put(const char *str) {
    while(char ch = *str++) {
        Put(ch);
    }
}

/**
 * Format the given string and print it to the console.
 *
 * @return Number of characters written to the console
 */
int Console::Print(const char *format, ...) {
    va_list va;
    va_start(va, format);
    const auto ret = vprintf(format, va);
    va_end(va);
    return ret;
}
