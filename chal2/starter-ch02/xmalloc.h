#ifndef HMALLOC_H
#define HMALLOC_H

// Husky Malloc Interface
// cs3650 Starter Code

#include <stddef.h>

typedef struct fl_cell {
    size_t size;
    struct fl_cell* next;
    struct fl_cell* last;
} fl_cell;

fl_cell* make_fl_cell(void* addr, size_t size);

void* xmalloc(size_t size);
void xfree(void* item);
void* xrealloc(void* item, size_t new_size);

#endif
