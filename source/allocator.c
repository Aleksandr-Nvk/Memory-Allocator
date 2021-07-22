#include <sys/mman.h>
#include "allocator.h"
#include "cache.h"

/* allocates memory of 'size' and returns it */
__attribute__((warn_unused_result)) void* alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    Cache* cache = tree_get_cache_of_size(size);    /* try to find cache that stores slabs of 'size' */

    if (cache == NULL) {
        cache = cache_new(size);    /* create a new cache if not found */
    }

    return cache_extract_mem(cache);    /* get memory from cache */
}

/* returns memory from ptr back to the allocator */
void sfree(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    tree_free_address(root, ptr);
}