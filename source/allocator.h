#pragma once

#include <stdlib.h>

/* Returns a pointer to a block of 'size' bytes of memory */
__attribute__((warn_unused_result)) void* alloc(size_t size);

void sfree(void* ptr);