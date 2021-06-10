#include <stdio.h>
#include <sys/mman.h>

#define CACHE_FREE_SIZE       8   /* amount of free slabs initially stored in the Head Cache */
#define CACHE_PARTIAL_SIZE    8   /* amount of partial slabs initially stored int the Head Cache */
#define CACHE_FULL_SIZE       8   /* amount of full slabs initially stored int the Head Cache */

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

/* Returns a new cache with specific data and arrays sizes */
static Cache* cache_new(size_t free_size, size_t partial_size, size_t full_size, size_t data_size) {
    Cache* cache = mmap(0, sizeof(Cache), ACCESS, VISIBILITY, -1, 0);
    cache->next = NULL;

    cache->free_size =       free_size;
    cache->partial_size =    partial_size;
    cache->full_size =       full_size;
    cache->data_size =       data_size;

    cache->free =       mmap(0, data_size * free_size, ACCESS, VISIBILITY, -1, 0);
    cache->partial =    mmap(0, data_size * partial_size, ACCESS, VISIBILITY, -1, 0);
    cache->full =       mmap(0, data_size * full_size, ACCESS, VISIBILITY, -1, 0);

    return cache;
}

/* Creates and initializes a new cache for types of 'data_size' length and adds it to the list of caches */
void cache_init(size_t free_size, size_t partial_size, size_t full_size, size_t data_size) {
    if (head == NULL) {     /* check if the Head Cache exist already and create it if not */
        tail = cache_new(CACHE_FREE_SIZE, CACHE_PARTIAL_SIZE, CACHE_FULL_SIZE, sizeof(Cache));
    }

    tail->next = cache_new(free_size, partial_size, full_size, data_size);
}