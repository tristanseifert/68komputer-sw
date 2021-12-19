#include "Console.h"
#include "hw/Xr68c681.h"

#include <printf.h>

/**
 * Writes a string to the console.
 */
void Console::Puts(const char *str) {
    while(char ch = *str++) {
        hw::Xr68C681::PutCharA(ch);
    }
}
