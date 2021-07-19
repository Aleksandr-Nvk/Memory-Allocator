#ifndef MEMORY_ALLOCATOR_ALLOCATOR_H
#define MEMORY_ALLOCATOR_ALLOCATOR_H

#include <sys/mman.h>

#define INIT_BUF_SIZE      8    /* initial amount of nodes for a buffer of slabs */

#define ACCESS          PROT_READ | PROT_WRITE          /* common access mode for allocated memory */
#define VISIBILITY      MAP_PRIVATE | MAP_ANONYMOUS     /* common visibility mode for allocated memory */

typedef enum State {
    freee,
    partial,
    full
} State;

/* Slab contains range of memory addresses. It is also a node of a linked list */
typedef struct Slab {
    struct Slab* next;

    size_t data_size;   /* size of data blocks stored in this slab */

    State state;

    void* start;    /* the first memory address */
    void* end;    /* the last memory address */
} Slab;

/* Cache is a node of a linked list. Contains lists of slabs of free, partial and full types */
typedef struct Cache {
    struct Cache* left;
    struct Cache* right;

    size_t data_size;

    Slab* free_buff;
    Slab* full_buff;
} Cache;

/* Returns a pointer to a block of 'size' bytes of memory */
__attribute__((warn_unused_result)) void* alloc(size_t size);

void add_cache(Cache* cache);
Cache* get_cache_of_size(size_t data_size);

#endif