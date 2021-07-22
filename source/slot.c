#include <stddef.h>
#include "slot.h"

/* creates a new slot of 'data_size' from 'mem' passed to this function, and returns it */
Slot* slot_new(size_t data_size, void* mem) {
    Slot* new_slot = mem;
    mem += sizeof(Slot);

    new_slot->next = NULL;
    new_slot->mem = mem;
    mem += data_size;

    return new_slot;
}

/* tries to find 'ptr' in a linked list of addresses (starting from 'current') and returns the slot. Returns NULL if not found */
Slot* slots_free_address(Slot* current, void* ptr) {
    while (current != NULL) {
        if (current->mem == ptr) {
            return current;
        }

        current = current->next;
    }

    return NULL;
}