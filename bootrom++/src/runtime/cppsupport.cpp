#include <new.h>

#include <stddef.h>
#include <stdint.h>

/*
 * Various support routines for C++
 */

extern "C" void __cxa_pure_virtual() {
    while (1);
}

// Placement new
void *operator new(size_t size, void *ptr) {
    (void) size;
    return ptr;
}

void *operator new[](size_t size, void *ptr) {
    (void) size;
    return ptr;
}
