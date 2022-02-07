#include "Console.h"

#include <printf.h>

using namespace io;

CharacterDevice *Console::gOutputDevice{nullptr};

/**
 * Thunk through the bare printf output to the console.
 */
void _putchar(char ch) {
    Console::Put(ch);
}

/**
 * Read a character, if any are pending.
 */
int Console::GetCharWrapper() {
    if(!GetCharPending()) return -1;
    return GetChar();
}




/**
 * Initializes the console system.
 */
void Console::Init() {
    // TODO: do stuff
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
