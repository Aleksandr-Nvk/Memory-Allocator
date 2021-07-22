#pragma once

typedef struct Slot {
    struct Slot* next;

    void* mem;
} Slot;

Slot* slot_new(size_t data_size, void* mem);