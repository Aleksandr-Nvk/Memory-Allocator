#ifndef MEMORY_ALLOCATOR_ALLOCATOR_H
#define MEMORY_ALLOCATOR_ALLOCATOR_H

#include <stdio.h>
#include <sys/mman.h>

#define ADDRESSES_IN_SLAB       4096    /* amount of addresses in one slab */
#define MIN_LIST_CAPACITY       4

#define ACCESS          PROT_READ | PROT_WRITE          /* common access mode for allocated memory */
#define VISIBILITY      MAP_PRIVATE | MAP_ANONYMOUS     /* common visibility mode for allocated memory */

/* Slab contains several pages of memory */
typedef struct Slab {
    void* start;    /* the first memory address */
    size_t addr_amount;    /* amount of addresses */
} Slab;

typedef struct List {
    Slab* array;

    size_t count;            /* amount of items currently stored in list */
    size_t capacity;            /* possible amount of items stored in list (can be expanded) */
} List;

/* Cache is a node of a free list. Contains arrays of slabs of free, partial and full types */
typedef struct Cache {
    struct Cache* next;
    size_t data_size;
    List free_slabs, partial_slabs, full_slabs;
} Cache;

/* Returns a pointer to a block of 'size' bytes of memory */
void* alloc(size_t size);

#endif