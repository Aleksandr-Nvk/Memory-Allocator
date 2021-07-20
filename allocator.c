#include <unistd.h>
#include "allocator.h"

Cache* root = NULL;    /* root of the binary tree of caches */

/* a wrap around the system call */
static void* sys_alloc(size_t size) {
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

/* adds 'cache' to the binary tree of caches, based on data_size in 'cache' */
static void tree_add_cache(Cache* cache) {
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

/* searches for a cache with 'data_size' slabs in the binary tree of caches and returns it (or NULL if not found) */
static Cache* get_cache_of_size(size_t data_size) {
    Cache* current = root;

    while (1) {
        if (current == NULL) {
            return NULL;
        }
        if (current->data_size == data_size) {
            return current;
        }

        current = data_size < current->data_size ? current->left : current->right;
    }
}

/* splits 'mem' and uses it to create 'amount' of slabs, which are added to 'cache' */
static void cache_add_free_slabs(Cache* cache, void* mem, size_t amount) {
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

/* creates a new cache for slabs containing 'data_size' memory pieces and adds it to the binary tree of caches */
static Cache* cache_new(size_t data_size) {
    size_t cache_struct_size = sizeof(Cache);
    size_t slab_struct_size = sizeof(Slab);
    size_t slab_size = getpagesize() * data_size;

    void* raw_mem = sys_alloc(cache_struct_size + (slab_size + slab_struct_size) * INIT_BUF_SIZE);

    Cache* new_cache = raw_mem;
    raw_mem += cache_struct_size;

    new_cache->left = new_cache->right = NULL;
    new_cache->data_size = data_size;

    cache_add_free_slabs(new_cache, raw_mem, INIT_BUF_SIZE);

    tree_add_cache(new_cache);

    return new_cache;
}

/* returns a pointer to memory of size, determined in 'cache' */
void* cache_extract_mem(Cache* cache) {
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

    void* mem = sys_alloc(sizeof(Slab) + getpagesize() * data_size * INIT_BUF_SIZE);
    cache_add_free_slabs(cache, mem, INIT_BUF_SIZE);

    return cache_extract_mem(cache);
}

__attribute__((warn_unused_result)) void* alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    Cache* cache = get_cache_of_size(size);

    if (cache == NULL) {
        cache = cache_new(size);
    }

    return cache_extract_mem(cache);
}