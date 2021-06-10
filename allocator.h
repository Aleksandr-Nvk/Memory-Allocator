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

/* Returns a pointer to a block of memory of 'size' bytes */
void* alloc(size_t size);

#endif