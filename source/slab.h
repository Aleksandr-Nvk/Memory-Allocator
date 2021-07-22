#pragma once

#include <stdlib.h>
#include "slot.h"

/* container for linked lists of used and unused slots, node of linked list of slabs */
typedef struct Slab {
    struct Slab* next;

    size_t data_size;

    Slot* used_slot_head;   /* linked list of used slots */
    Slot* unused_slot_head;    /* linked list of unused slots */
} Slab;

/* creates a new slab of 'data_size' from 'mem' passed to this function, and returns it */
Slab* slab_new(size_t data_size, void* mem);

/* returns free memory, extracted from 'slab' */
void* slab_extract_mem(Slab* slab);

/* tries to free 'ptr' from every slab in a linked list, starting from 'current'. Returns the slab on success and NULL on failure */
Slab* slabs_free_address(Slab* current, void* ptr);