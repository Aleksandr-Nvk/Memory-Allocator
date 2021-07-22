#include "slab.h"
#include "cache.h"
#include "slot.h"

Slab* slab_new(size_t data_size, void* mem) {
    Slab* new_slab = mem;
    mem += sizeof(Slab);

    new_slab->data_size = data_size;
    new_slab->next = NULL;
    new_slab->used_slot_head = new_slab->unused_slot_head = NULL;

    for (int i = 0; i < page_size; ++i) {
        Slot* new_slot = slot_new(data_size, mem);
        mem += sizeof(Slot) + data_size;

        new_slot->next = new_slab->unused_slot_head;
        new_slab->unused_slot_head = new_slot;
    }

    return new_slab;
}

void* slab_extract_mem(Slab* slab) {
    void* mem = slab->unused_slot_head->mem;

    Slot* slot = slab->unused_slot_head;
    slab->unused_slot_head = slab->unused_slot_head->next;

    slot->next = slab->used_slot_head;
    slab->used_slot_head = slot;

    return mem;
}