#include <sys/mman.h>
#include "allocator.h"
#include "cache.h"

__attribute__((warn_unused_result)) void* alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    Cache* cache = tree_get_cache_of_size(size);

    if (cache == NULL) {
        cache = cache_new(size);
    }

    return cache_extract_mem(cache);
}