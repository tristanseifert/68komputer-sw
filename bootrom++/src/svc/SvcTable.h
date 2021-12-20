#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Service table contains pointers to all functions implemented by the ROM. These can be invoked
 * directly by code running on the system.
 */
struct SvcTable {
    /// Magic value: should be 'FUNC'
    uint32_t magic{'FUNC'};

    /**
     * Version of the table; this defines what methods are available.
     */
    uint32_t version;

    /// @{ @name Console
    /**
     * Outputs the given string to the console.
     *
     * @param str String to output; must be NULL terminated
     */
    void (*ConsPuts)(const char *str);

    /**
     * Prints the given formatted string to the console.
     *
     * @param format Format string, printf() style
     * @param ... Arguments to format
     *
     * @return Number of characters to output
     */
    int (*ConsPrint)(const char *format, ...) __attribute__((format(printf, 1, 2)));
    /// @}

    /// @{ @name String
    /**
     * Formats the given string into the string buffer.
     *
     * @param buffer The character buffer to receive the formatted string
     * @param bufferLen Size of buffer, in bytes
     * @param format Format string
     * @param ... Arguments to format
     *
     * @return Number of characters that could have been written or -1 on error
     */
    int (*StrFormatBufferList)(char *buffer, size_t bufferLen, const char *format, va_list va);

    /**
     * Formats the given string into the string buffer.
     *
     * @param buffer The character buffer to receive the formatted string
     * @param bufferLen Size of buffer, in bytes
     * @param format Format string
     * @param ... Arguments to format
     *
     * @return Number of characters that could have been written or -1 on error
     */
    int (*StrFormatBuffer)(char *buffer, size_t bufferLen, const char *format, ...) __attribute__((format(printf, 3, 4)));
    /// @}

    /// Global instance of the service table
    static const SvcTable gTable;
};
