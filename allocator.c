#include "allocator.h"
#include <unistd.h>

Cache* head = NULL;    /* head of the linked list */

static Cache* get_tail_cache(Cache* current) {
    while (current->next != NULL) {
        current = current->next;
    }

    return current;
}

static Slab* get_tail_slab(Slab* current, int remove) {
    Slab* previous;
    while (current->next != NULL) {
        previous = current;
        current = current->next;
    }

    if (remove) {
        previous->next = NULL;
    }
    
    return current;
}

static void add_tail_slab(Slab* current, Slab* to_add) {
    get_tail_slab(current, 0)->next = to_add;
}

static void* mem_slice(void** mem, size_t* mem_size, size_t requested_size) {
    if (requested_size <= *mem_size) {
        void* mem_cached = *mem;
        *mem_size -= requested_size;
        *mem += requested_size;
        return mem_cached;
    }

    perror("NOT ENOUGH MEMORY");
    return NULL;
}

static Slab* slab_new(void* mem, size_t* all_size, size_t slab_size, size_t slab_mem_size, size_t data_size) {
    Slab* new_slab = mem_slice(&mem, all_size, slab_size);
    new_slab->next = NULL;
    new_slab->data_size = data_size;
    new_slab->start = mem_slice(&mem, all_size, slab_mem_size);
    new_slab->end = new_slab->start + slab_mem_size - 1;

    return new_slab;
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
        Slab* new_slab = slab_new(&mem, &all_size, slab_size, slab_mem_size, data_size);

        if (new_cache->free_buf_head == NULL) {
            new_cache->free_buf_head = new_slab;
        } else {
            add_tail_slab(new_cache->free_buf_head, new_slab);
        }
    }

    if (head == NULL) {
        return head = new_cache;
    }

    return get_tail_cache(head)->next = new_cache;
}

static void* get_data(Cache* cache) {
    Slab* slab;
    void* object;
    
    if (cache->free_buf_head != NULL) {
        slab = get_tail_slab(cache->free_buf_head, 1);    /* DEBUGGER INTERRUPTS AT THIS LINE */
        
        if (cache->partial_buf_head != NULL) {
            add_tail_slab(cache->partial_buf_head, slab);
        } else {
            cache->partial_buf_head = slab;
        }
    } else if (cache->partial_buf_head != NULL) {
        slab = get_tail_slab(cache->partial_buf_head, 0);
        
        if (slab->start > slab->end) {
            get_tail_slab(cache->partial_buf_head, 1);
        }
    } else {
        size_t slab_size = sizeof(Slab);
        size_t slab_mem_size = cache->data_size * getpagesize() / 8;
        size_t all_size = slab_size + slab_mem_size;
        void* mem = alloc(all_size);
        slab = slab_new(&mem, &all_size, slab_size, slab_mem_size, cache->data_size);

        if (cache->partial_buf_head != NULL) {
            add_tail_slab(cache->partial_buf_head, slab);
        } else {
            cache->partial_buf_head = slab;
        }
    }

    object = slab->start;
    slab->start += slab->data_size;

    return object;
}

/* Returns a pointer to a block of 'size' bytes of memory */
void* alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    Cache* cache = head;

    while (cache != NULL) {
        if (cache->data_size == size) {
            goto found;    /* jump over one excessive NULL-check */
        }

        cache = cache->next;
    }

    cache = cache_new(size);

    found:;

    return get_data(cache);
}