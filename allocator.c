#include "allocator.h"
#include <unistd.h>

static Cache* head;    /* head of the linked list */
static Cache* tail;    /* tail of the linked list */

/* ONLY OUTSIDE CACHES!!! */
static Slab* slab_alloc() {}

static Slab* get_tail_slab(Slab* current) {
    while (current->next != NULL) {
        current = current->next;
    }

    return current;
}

static void* mem_slice(void** mem, size_t* mem_size, size_t requested_size) {
    if (requested_size <= *mem_size) {
        *mem_size -= requested_size;
        *mem += requested_size;
        return *mem;
    }

    perror("NOT ENOUGH MEMORY!");
    return NULL;
}

/* Creates a new cache for 'data_size'-length types, adds it to the linked list and returns it */
static Cache* cache_new(size_t data_size) {
    size_t cache_size = sizeof(Cache);
    size_t slab_size = sizeof(Slab);
    size_t slab_mem_size = data_size * getpagesize() / 8;
    size_t all_size = cache_size + (slab_size + slab_mem_size) * INIT_FREE_BUF_SIZE;

    void* mem = mmap(NULL, all_size, ACCESS, VISIBILITY, -1, 0);

    Cache* new_cache = mem_slice(&mem, &all_size, cache_size);

    new_cache->next = NULL;
    new_cache->data_size = data_size;

    new_cache->free_buf_head = NULL;
    new_cache->partial_buf_head = NULL;
    new_cache->full_buf_head = NULL;

    for (int i = 0; i < INIT_FREE_BUF_SIZE; ++i) {
        Slab* new_slab = mem_slice(&mem, &all_size, slab_size);
        new_slab->next = NULL;
        new_slab->data_size = data_size;
        new_slab->is_full = 0;
        new_slab->start = mem_slice(&mem, &all_size, slab_mem_size);
        new_slab->end = new_slab->start + slab_mem_size - 1;

        if (new_cache->free_buf_head == NULL) {
            new_cache->free_buf_head = new_slab;
        } else {
            get_tail_slab(new_cache->free_buf_head)->next = new_slab;
        }
    }

    if (head == NULL) {
        head = tail = new_cache;
    } else {
        tail->next = new_cache;
    }

    return new_cache;
}

/* Returns a pointer to a block of 'size' bytes of memory */
void* alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    cache_new(size);
}