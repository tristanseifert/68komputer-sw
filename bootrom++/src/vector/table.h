#pragma once

#include <stddef.h>
#include <stdint.h>

class Vectors {
    private:
        /// Total number of vectors to reserve space for
        constexpr static const size_t kNumVectors{256};
        static const void *gVectorTable[kNumVectors];
};
