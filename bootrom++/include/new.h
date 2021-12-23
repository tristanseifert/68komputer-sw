#pragma once

#include <stddef.h>
#include <stdint.h>

// placement new
void *operator new(size_t size, void *ptr);
void *operator new[](size_t size, void *ptr);

void *operator new(size_t size, int type);
void *operator new[](size_t size, int type);
