#include <unistd.h>
#include "allocator.h"

Cache* root = NULL;    /* root of a binary tree of caches */

void* sys_alloc(size_t size) {
    return mmap(NULL, size, ACCESS, VISIBILITY, -1, 0);
}

void add_cache(Cache* cache) {
    if (root == NULL) {
        root = cache;
        return;
    }

    Cache* current = root;

    while (1) {
        if (cache->data_size < current->data_size) {
            if (current->left == NULL) {
                current->left = cache;
                return;
            }
            current = current->left;
        } else {
            if (current->right == NULL) {
                current->right = cache;
                return;
            }
            current = current->right;
        }
    }
}

Cache* get_cache_of_size(size_t data_size) {
    Cache* current = root;

    while (1) {
        if (current == NULL) {
            return NULL;
        }
        if (current->data_size == data_size) {
            return current;
        }

        if (data_size < current->data_size) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
}

void cache_add_free_slabs(Cache* cache, void* mem, size_t amount) {
    size_t slab_struct_size = sizeof(Slab);
    size_t slab_size = getpagesize() * cache->data_size;

    for (int i = 0; i < amount; ++i) {
        Slab* new_slab = mem;
        mem += slab_struct_size;

        new_slab->next = cache->free_buff;
        cache->free_buff = new_slab;
        new_slab->data_size = cache->data_size;
        new_slab->state = freee;

        new_slab->start = mem;
        mem += slab_size;
        new_slab->end = mem - 1;
    }
}

Cache* cache_new(size_t data_size) {
    size_t cache_struct_size = sizeof(Cache);
    size_t slab_struct_size = sizeof(Slab);
    size_t slab_size = getpagesize() * data_size;

    void* raw_mem = sys_alloc(cache_struct_size + (slab_size + slab_struct_size) * INIT_BUF_SIZE);

    Cache* new_cache = raw_mem;
    raw_mem += cache_struct_size;

    new_cache->left = new_cache->right = NULL;
    new_cache->data_size = data_size;

    cache_add_free_slabs(new_cache, raw_mem, INIT_BUF_SIZE);

    add_cache(new_cache);

    return new_cache;
}

void* get_data(Cache* cache) {
    size_t data_size = cache->data_size;
    Slab* slab = cache->free_buff;

    if (slab != NULL) {
        void* data = slab->start;
        slab->start += data_size;

        if (slab->start > slab->end) {
            slab->state = full;
            cache->free_buff = cache->free_buff->next;

            slab->next = cache->full_buff;
            cache->full_buff = slab;
        } else {
            slab->state = partial;
        }
        return data;
    }

    cache_add_free_slabs(cache, sys_alloc((sizeof(Slab) + getpagesize() * data_size * INIT_BUF_SIZE)), INIT_BUF_SIZE);

    return get_data(cache);
}

__attribute__((warn_unused_result)) void* alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    Cache* cache = get_cache_of_size(size);

    if (cache == NULL) {
        cache = cache_new(size);
    }

    return get_data(cache);
}