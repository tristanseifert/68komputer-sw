#pragma once

#include <stdarg.h>
#include <stddef.h>

class Console {
    public:
        static void Init();

        static void Put(const char ch);
        static void Put(const char *);
        static int Print(const char* format, ...) __attribute__ ((format (printf, 1, 2)));
};
