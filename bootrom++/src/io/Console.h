#pragma once

#include <stdarg.h>
#include <stddef.h>

class Console {
    public:
        static void Puts(const char *);
        static void Print(const char* format, ...);
};
