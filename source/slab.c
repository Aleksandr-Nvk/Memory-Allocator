#include "slab.h"
#include "cache.h"
#include "slot.h"

/* creates a new slab of 'data_size' from 'mem' passed to this function, and returns it */
Slab* slab_new(size_t data_size, void* mem) {
    Slab* new_slab = mem;
    mem += sizeof(Slab);

    new_slab->data_size = data_size;
    new_slab->next = NULL;
    new_slab->used_slot_head = new_slab->unused_slot_head = NULL;

    /* add 4096 (usually) slots to the new slab */
    for (int i = 0; i < page_size; ++i) {
        Slot* new_slot = slot_new(data_size, mem);
        mem += sizeof(Slot) + data_size;

        new_slot->next = new_slab->unused_slot_head;
        new_slab->unused_slot_head = new_slot;
    }

    return new_slab;
}

/* returns free memory, extracted from 'slab' */
void* slab_extract_mem(Slab* slab) {
    void* mem = slab->unused_slot_head->mem;    /* extract */

    /* remove the slot from unused slots */
    Slot* slot = slab->unused_slot_head;
    slab->unused_slot_head = slab->unused_slot_head->next;

    /* put the slot to used slots */
    slot->next = slab->used_slot_head;
    slab->used_slot_head = slot;

    return mem;
}

/* tries to free 'ptr' from every slab in a linked list, starting from 'current'. Returns the slab on success and NULL on failure */
Slab* slabs_free_address(Slab* current, void* ptr) {
    while (current != NULL) {
        Slot* slot = slots_free_address(current->used_slot_head, ptr);

        /* move the slot to used slots if freed successfully */
        if (slot != NULL) {
            slot->next = current->unused_slot_head;
            current->unused_slot_head = slot;

            return current;
        }

        current = current->next;
    }

    return NULL;
}