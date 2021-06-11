#include "allocator.h"

/* returns a new blank list */
List list_new(void) {
    List new_list = {NULL, 0, 0};
    return new_list;
}

/* returns an item from a list by an index (or empty if the index is out of range) */
Slab list_of_index(List* list, size_t index) {
    if (index < list->count || index >= 0) {
        return list->array[index]; \
    }

    /* Throw the following error if the index is out of range */
    perror("\nINDEX WAS OUT OF RANGE");

    Slab result = {0};
    return result;
}

/* adds an item to a list */
void list_add(List* list, Slab item) {
    if (list->capacity == 0) {
        list->capacity = MIN_LIST_CAPACITY;
        list->array = mmap(NULL, list->capacity * sizeof(Slab), ACCESS, VISIBILITY, -1, 0);
    } else if (list->count == list->capacity) {
        munmap(list->array, list->capacity);
        list->capacity *= 2;            /* double capacity of list if more space is needed */
        list->array = mmap(NULL, list->capacity * sizeof(Slab), ACCESS, VISIBILITY, -1, 0);
    }

    list->array[list->count++] = item;
}

/* removes the last item from the list and returns it */
Slab list_get_last(List* list) {
    Slab result = {0};

    if (list->count > 0) {
        result = list->array[list->count-- - 1];
    }

    return result;
}

/* removes the first occurrence of given item from a list */
void list_remove(List* list, Slab item) {
    for (int i = 0; i < list->count; ++i) {
        if ((list->array[i].start == item.start && list->array[i].addr_amount == item.addr_amount)) {
            /* Defragmentation of the list */
            for (int k = i; k < list->count - 1; ++k) {
                list->array[k] = list->array[k + 1];
            }
            --list->count;
            break;
        }
    }
}

/* removes all items from a list (capacity is unchanged) */
void list_clear(List* list) {
    list->count = 0;
}

void list_dealloc(List* list) {
    list->count = 0;

    munmap(list->array, list->capacity);

    list->capacity = 0;
}