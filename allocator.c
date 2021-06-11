#include "list.h"
#include "allocator.h"

static Cache* head;     /* head of the free list. Always the Head Cache */
static Cache* tail;     /* tail of the free list */

Slab slab_new(size_t data_size) {
    size_t size = ADDRESSES_IN_SLAB * data_size;
    void* cache_start = mmap(NULL, size, ACCESS, VISIBILITY, -1, 0);    /* allocate memory for each slab */
    Slab new_slab = {cache_start, size};

    return new_slab;
}

/* Returns a new cache with specific data size */
static Cache* cache_new(size_t data_size) {
    Cache* cache = mmap(NULL, sizeof(Cache), ACCESS, VISIBILITY, -1, 0);
    cache->next = NULL;

    cache->data_size = data_size;

    cache->free_slabs = list_new();
    cache->partial_slabs = list_new();
    cache->full_slabs = list_new();

    /* fill the array for free slabs */
    for (int i = 0; i < cache->free_slabs.capacity; ++i) {
        size_t size = ADDRESSES_IN_SLAB * data_size;
        void* cache_start = mmap(NULL, size, ACCESS, VISIBILITY, -1, 0);    /* allocate memory for each slab */

        Slab new_slab = slab_new(size);
        list_add(&cache->free_slabs, new_slab);
    }

    return cache;
}

/* Returns a pointer to a block of 'size' bytes of memory */
void* alloc(size_t size) {
    Cache *cache;

    if (size == 0) {
        return NULL;
    }

    if (head == NULL) {     /* if the Head Cache is NULL, then there are no caches at all */
        head = cache_new(sizeof(Cache));
        head->next = tail = cache = cache_new(size);
    } else {
        cache = head->next;

        /* search for cache with matching data size */
        while (cache != NULL) {
            if (cache->data_size == size) {
                cache = cache;
                goto found;     /* jump over one excessive NULL-check */
            } else {
                cache = cache->next;
            }
        }

        tail->next = cache = cache_new(size);   /* create a new cache if none was found */
    }

    found:;

    void* object;

    if (cache->free_slabs.count > 0) {
        Slab free_slab = list_get_last(&cache->free_slabs);
        free_slab.addr_amount -= size;
        object = free_slab.start + free_slab.addr_amount - 1;

        list_add(&cache->partial_slabs, free_slab);

    } else if (cache->partial_slabs.count > 0) {
        Slab free_slab = list_get_last(&cache->partial_slabs);
        free_slab.addr_amount -= size;
        object = free_slab.start + free_slab.addr_amount - 1;

        if (free_slab.addr_amount == 0) {
            list_add(&cache->full_slabs, free_slab);
        }
    } else {
        Slab free_slab = slab_new(size);
        free_slab.addr_amount -= size;
        object = free_slab.start + free_slab.addr_amount - 1;

        list_add(&cache->partial_slabs, free_slab);
    }

    return object;
}