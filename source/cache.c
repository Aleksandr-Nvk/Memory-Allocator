#include "cache.h"
#include <sys/mman.h>
#include <unistd.h>
#include <printf.h>

Cache* root = NULL;    /* root of the binary tree of caches */
size_t page_size;

/* a wrap around the system call */
void* sys_alloc(size_t size) {
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

/* adds 'cache' to the binary tree of caches, based on data_size in 'cache' */
void tree_add_cache(Cache* cache) {
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
Cache* tree_get_cache_of_size(size_t data_size) {
    Cache* current = root;

    /* standard binary tree search */
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

/* creates a new cache, adds it to the tree of caches and returns it */
Cache* cache_new(size_t data_size) {
    if (page_size == 0) {
        page_size = getpagesize();    /* do this once, since getpagesize() is a sys call */
    }

    size_t slab_size = sizeof(Slab) + page_size * (sizeof(Slot) + data_size);

    void* mem = sys_alloc(sizeof(Cache) + INIT_BUF_SIZE * (slab_size));

    Cache* new_cache = mem;
    mem += sizeof(Cache);

    new_cache->right = new_cache->left = NULL;
    new_cache->data_size = data_size;
    new_cache->free_slab_head = new_cache->partial_slab_head = new_cache->full_slab_head = NULL;

    /* add default amount of slabs to the new cache */
    for (int i = 0; i < INIT_BUF_SIZE; ++i) {
        Slab* new_slab = slab_new(data_size, mem);
        mem += slab_size;

        new_slab->next = new_cache->free_slab_head;
        new_cache->free_slab_head = new_slab;
    }

    tree_add_cache(new_cache);

    return new_cache;
}

/* extracts memory from 'cache' and manages slabs in it, depending on a condition */
void* cache_extract_mem(Cache* cache) {
    void* mem;

    if (cache->partial_slab_head != NULL) {
        // get from partial slabs
        Slab* slab = cache->partial_slab_head;
        mem = slab_extract_mem(slab);

        if (slab->unused_slot_head == NULL) {
            cache->partial_slab_head = cache->partial_slab_head->next;

            slab->next = cache->full_slab_head;
            cache->full_slab_head = slab;
        }
    } else if (cache->free_slab_head != NULL) {
        // get from free slabs
        Slab* slab = cache->free_slab_head;
        mem = slab_extract_mem(slab);

        cache->free_slab_head = cache->free_slab_head->next;

        slab->next = cache->partial_slab_head;
        cache->partial_slab_head = slab;
    } else {
        // add new slabs and call this function again
        size_t slab_size = (sizeof(Slab) + page_size * (sizeof(Slot) + cache->data_size));
        void* extra_mem = sys_alloc(INIT_BUF_SIZE * slab_size);

        for (int i = 0; i < INIT_BUF_SIZE; ++i) {
            Slab* new_slab = slab_new(cache->data_size, extra_mem);
            extra_mem += slab_size;

            new_slab->next = cache->free_slab_head;
            cache->free_slab_head = new_slab;
        }

        mem = cache_extract_mem(cache);
    }

    return mem;
}

/* traverses the tree and looks for a cache that contains 'ptr' in slots in its slabs */
void tree_free_address(Cache* current, void* ptr) {
    if (current != NULL) {
        /* try to free in partial slabs */
        Slab* freed = slabs_free_address(current->partial_slab_head, ptr);

        /* freed successfully */
        if (freed != NULL) {
            /* put the slab to full slabs if all of its slots are used */
            if (freed->used_slot_head == NULL) {
                freed->next = current->free_slab_head;
                current->free_slab_head = freed;

                return;
            }
        } else {
            /* try to free in full slabs */
            freed = slabs_free_address(current->full_slab_head, ptr);

            /* move the slab to partial if freed successfully */
            if (freed != NULL) {
                freed->next = current->partial_slab_head;
                current->partial_slab_head = freed;

                return;
            }
        }

        /* standard binary tree recursion (traversing) */
        tree_free_address(current->left, ptr);
        tree_free_address(current->right, ptr);
    }
}