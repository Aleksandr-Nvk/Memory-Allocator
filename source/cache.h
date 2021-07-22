#pragma once

#include "slab.h"

#define INIT_BUF_SIZE      8    /* initial amount of slabs in linked list of free slabs */

/* Cache is a node of a binary tree. Contains linked lists of slabs of free, partial and full types */
typedef struct Cache {
    struct Cache* left;
    struct Cache* right;

    size_t data_size;

    Slab* free_slab_head;
    Slab* partial_slab_head;
    Slab* full_slab_head;
} Cache;

extern Cache* root;     /* root of the binary tree of caches */
extern size_t page_size;    /* system's size of memory page. Calculated only once */

/* a wrap around the system call */
void* sys_alloc(size_t size);

/* adds 'cache' to the binary tree of caches, based on data_size in 'cache' */
void tree_add_cache(Cache* cache);

/* searches for a cache with 'data_size' slabs in the binary tree of caches and returns it (or NULL if not found) */
Cache* tree_get_cache_of_size(size_t data_size);

/* creates a new cache, adds it to the tree of caches and returns it */
Cache* cache_new(size_t data_size);

/* extracts memory from 'cache' and manages slabs in it, depending on a condition */
void* cache_extract_mem(Cache* cache);

/* traverses the tree and looks for a cache that contains 'ptr' in slots in its slabs */
void tree_free_address(Cache* current, void* ptr);