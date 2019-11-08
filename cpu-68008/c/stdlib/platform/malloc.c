/**
 * Provides wrappers around tinyalloc for the malloc/free functions.
 */
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "umm_malloc.h"

/**
 * Allocates a memory block and returns its address, or NULL if failed.
 */
void *malloc(size_t size) {
  return umm_malloc(size);
}

/**
 * Allocates multiple elements of the same size
 */
void *calloc(size_t count, size_t size) {
  return umm_calloc(count, size);
}

/**
 * Frees a memory block. In case this fails, we sit here forever.
 */
void free(void *block) {
  umm_free(block);
}

/**
 * Re-allocates a block to a different size while preserving data.
 */
void *realloc(void *ptr, size_t size) {
  return umm_realloc(ptr, size);
}
