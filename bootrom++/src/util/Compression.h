#pragma once

#include <stddef.h>
#include <stdint.h>

namespace util {
/**
 * Helper functions for working with compressed data.
 */
class Compression {
    public:
        static size_t DecompressLz4(const void *in, void *out);
};
}
