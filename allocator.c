#include <stdio.h>
#include <sys/mman.h>

#define CACHE_FREE_SIZE       8   /* amount of free slabs initially stored in caches */
#define CACHE_PARTIAL_SIZE    8   /* amount of partial slabs initially stored in caches */
#define CACHE_FULL_SIZE       8   /* amount of full slabs initially stored in caches */

#define ACCESS          PROT_READ | PROT_WRITE          /* common access mode for allocated memory */
#define VISIBILITY      MAP_PRIVATE | MAP_ANONYMOUS     /* common visibility mode for allocated memory */

/* Slab contains several pages of memory */
typedef struct Slab {
    void* start;    /* the first memory address */
    size_t size;    /* amount of addresses */
} Slab;

/* Cache is a node of a free list. Contains arrays of slabs of free, partial and full types */
typedef struct Cache {
    struct Cache* next;
    size_t free_size, partial_size, full_size, data_size;
    Slab *free, *partial, *full;
} Cache;

static Cache* head;     /* head of the free list. Always the Head Cache */
static Cache* tail;     /* tail of the free list */

/* Returns a new cache with specific data size */
static Cache* cache_new(size_t data_size) {
    Cache* cache = mmap(0, sizeof(Cache), ACCESS, VISIBILITY, -1, 0);
    cache->next = NULL;

    cache->free_size =       CACHE_FREE_SIZE;
    cache->partial_size =    CACHE_PARTIAL_SIZE;
    cache->full_size =       CACHE_FULL_SIZE;
    cache->data_size =       data_size;

    cache->free =       mmap(0, data_size * CACHE_FREE_SIZE, ACCESS, VISIBILITY, -1, 0);
    cache->partial =    mmap(0, data_size * CACHE_PARTIAL_SIZE, ACCESS, VISIBILITY, -1, 0);
    cache->full =       mmap(0, data_size * CACHE_FULL_SIZE, ACCESS, VISIBILITY, -1, 0);

    return cache;
}

/* Returns a pointer to a block of memory of 'size' bytes */
void* alloc(size_t size) {
    Cache *cache;

    if (size == 0) {
        return NULL;
    }

    if (head == NULL) {     /* if the Head Cache is NULL, then there are no caches at all */
        head = cache_new(sizeof(Cache));
        head->next = tail = cache = cache_new(size);
        goto found;     /* jump over searching process */
    }

    cache = head->next;

    /* search for cache with matching data size */
    while (cache != NULL) {
        if (cache->data_size == size) {
            cache = cache;
            goto found;     /* jump over one excessive NULL-check */
        }

        cache = cache->next;
    }

    tail->next = cache = cache_new(size);   /* create a new cache if none was found */

    found:
    // look for a free object in 'cache'
}