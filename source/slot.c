#include <stddef.h>
#include "slot.h"

Slot* slot_new(size_t data_size, void* mem) {
    Slot* new_slot = mem;
    mem += sizeof(Slot);

    new_slot->next = NULL;
    new_slot->mem = mem;
    mem += data_size;

    return new_slot;
}