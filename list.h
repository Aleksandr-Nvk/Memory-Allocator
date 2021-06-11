#ifndef MEMORY_ALLOCATOR_LIST_H
#define MEMORY_ALLOCATOR_LIST_H

#include "allocator.h"

/* returns a new blank list */
List list_new(void);

/* returns an item from a list by an index (or NULL if the index is out of range) */
Slab list_of_index(List* list, size_t index);

/* adds an item to a list */
void list_add(List* list, Slab item);

/* removes the last item from the list and returns it */
Slab list_get_last(List* list);

/* removes the first occurrence of given item from a list */
void list_remove(List* list, Slab item);

/* removes all items from a list (capacity is unchanged) */
void list_clear(List* list);

/* completely deallocates a list */
void list_dealloc(List* list);

#endif
