#pragma once

/* container for single piece of memory, node of linked list of slots */
typedef struct Slot {
    struct Slot* next;

    void* mem;
} Slot;

/* creates a new slot of 'data_size' from 'mem' passed to this function, and returns it */
Slot* slot_new(size_t data_size, void* mem);

/* tries to find 'ptr' in a linked list of addresses (starting from 'current') and returns the slot. Returns NULL if not found */
Slot* slots_free_address(Slot* current, void* ptr);