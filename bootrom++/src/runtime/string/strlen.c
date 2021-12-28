#include <string.h>

/**
 *  * Simple naive implementation
 *   */
size_t strlen(const char* str) {
    size_t len = 0;

    while(*str++ != '\0') {
        len++;
    }

    return len;
}
