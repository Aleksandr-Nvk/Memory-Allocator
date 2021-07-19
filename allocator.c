#include "allocator.h"
#include <unistd.h>

Cache* head = NULL;    /* head of the linked list */
static int nodes;

Cache* get_tail_cache() {
    if (head == NULL) {
        return NULL;
    }

    Cache* current = head;

    while (current->next != NULL) {
        current = current->next;
    }

    return current;
}

void remove_tail_slab(Slab** head_slab) {
    if ((*head_slab)->next == NULL) {
        *head_slab = NULL;
        return;
    }

    Slab* current = *head_slab;

    while (current->next->next != NULL) {
        current = current->next;
    }

    current->next = NULL;
}

Slab* get_tail_slab(Slab* current) {
    if (current == NULL) {
        return NULL;
    }

    while (current->next != NULL) {
        current = current->next;
    }

    return current;
}

/* Creates a new cache for 'data_size'-length types, adds it to the linked list and returns it */
static Cache* cache_new(size_t data_size) {
    size_t cache_struct_size = sizeof(Cache);
    size_t slab_struct_size = sizeof(Slab);
    size_t slab_content_size = getpagesize() * data_size;

    size_t all_size = (slab_content_size + slab_struct_size) * INIT_BUF_SIZE + cache_struct_size;

    void* raw_mem = mmap(NULL, all_size, ACCESS, VISIBILITY, -1, 0);

    Cache* new_cache = raw_mem;
    raw_mem += cache_struct_size;

    if (head == NULL) {
        head = new_cache;
    } else {
        get_tail_cache()->next = new_cache;
    }

    new_cache->data_size = data_size;
    new_cache->next = NULL;
    new_cache->free_buf_head = new_cache->partial_buf_head = new_cache->full_buf_head = NULL;

    for (int i = 0; i < INIT_BUF_SIZE; ++i) {
        Slab* new_slab = raw_mem;
        raw_mem += slab_struct_size;

        new_slab->next = NULL;
        new_slab->data_size = data_size;

        new_slab->start = raw_mem;
        raw_mem += slab_content_size;
        new_slab->end = raw_mem - 1;

        if (new_cache->free_buf_head == NULL) {
            new_cache->free_buf_head = new_slab;
        } else {
            get_tail_slab(new_cache->free_buf_head)->next = new_slab;
        }
    }
    ++nodes;
    return new_cache;
}

void* get_data(Cache* cache) {
    size_t data_size = cache->data_size;
    void* data;

    if (cache->free_buf_head != NULL) {
        Slab* slab = get_tail_slab(cache->free_buf_head);
        data = slab->start;
        slab->start += data_size;

        if (cache->partial_buf_head == NULL) {
            cache->partial_buf_head = slab;
        } else {
            get_tail_slab(cache->partial_buf_head)->next = slab;
        }

        remove_tail_slab(&cache->free_buf_head);

    } else if (cache->partial_buf_head != NULL) {
        Slab* slab = get_tail_slab(cache->partial_buf_head);
        data = slab->start;
        slab->start += data_size;

        if (slab->start > slab->end) {
            if (cache->full_buf_head == NULL) {
                cache->full_buf_head = slab;
            } else {
                get_tail_slab(cache->full_buf_head)->next = slab;
            }

            remove_tail_slab(&cache->partial_buf_head);
        }
    } else {
        data = get_data(cache_new(data_size));
    }

    return data;
}

/* Returns a pointer to a block of 'size' bytes of memory */
__attribute__((warn_unused_result)) void* alloc(size_t size) {
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