#pragma once

#include "slab.h"

#define INIT_BUF_SIZE      8    /* initial amount of nodes for a buffer of slabs */

/* Cache is a node of a linked list. Contains lists of slabs of free, partial and full types */
typedef struct Cache {
    struct Cache* left;
    struct Cache* right;

    size_t data_size;

    Slab* free_slab_head;
    Slab* partial_slab_head;
    Slab* full_slab_head;
} Cache;

extern Cache* root;
extern size_t page_size;

void* sys_alloc(size_t size);

void tree_add_cache(Cache* cache);

Cache* tree_get_cache_of_size(size_t data_size);

Cache* cache_new(size_t data_size);

void* cache_extract_mem(Cache* cache);