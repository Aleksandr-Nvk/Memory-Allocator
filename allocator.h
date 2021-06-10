#ifndef MEMORY_ALLOCATOR_ALLOCATOR_H
#define MEMORY_ALLOCATOR_ALLOCATOR_H

#include <stdio.h>

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

/* Creates and initializes a new cache for types of 'data_size' length and adds it to the list of caches */
void cache_init(size_t free_size, size_t partial_size, size_t full_size, size_t data_size);

#endif