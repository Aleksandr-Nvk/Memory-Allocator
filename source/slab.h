#pragma once

#include <stdlib.h>
#include "slot.h"

/* Slab contains range of memory addresses. It is also a node of a linked list */
typedef struct Slab {
    struct Slab* next;

    size_t data_size;   /* size of data blocks stored in this slab */

    Slot* used_slot_head;
    Slot* unused_slot_head;
} Slab;

Slab* slab_new(size_t data_size, void* mem);

void* slab_extract_mem(Slab* slab);