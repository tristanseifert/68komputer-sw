#include "Compression.h"

using namespace util;

/**
 * Decompresses an LZ4 blob. We do this by jumping into the assembler LZ4 depack frame routine.
 *
 * @return Number of bytes decompressed; 0 indicates an error.
 */
size_t Compression::DecompressLz4(const void *in, void *out) {
    size_t outBytes{0};

    asm volatile(R"(
    mov.l       %[inPtr], %%a0
    mov.l       %[outPtr], %%a1
    bsr         lz4_frame_depack
    mov.l       %%d0, %[outBytes]
    )" : [outBytes] "=g"(outBytes) : [inPtr] "g"(in), [outPtr] "g"(out) :
    /* clobbers for fast lz4_depack */
    "d0","d1","d2","d3","d5","d7","a0","a1","a2","a3","a4");
    /* clobbers for the normal lz4_depack */
    //"d0","d1","d2","d3","d4","a0","a1","a3","a4");

    return outBytes;
}

