// hmalloc.c

#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>

#include "hmalloc.h"

/*
  typedef struct hm_stats {
    long pages_mapped;
    long pages_unmapped;
    long chunks_allocated;
    long chunks_freed;
    long free_length;
  } hm_stats;

  // Free list cell
  // min allocation is a user request of 8bytes, so anything smaller
  // needs to be rounded up
  typedef struct fl_cell {
    void* addr; //Address of this cell
    size_t size;
    fl_cell* next;
  } fl_cell;
*/

const size_t PAGE_SIZE = 4096;
static hm_stats stats; // This initializes the stats to 0.
static fl_cell head; // I dont want to have to initialize head every single time...
                     // but how do i make sure that the entire free list is after it?

fl_cell* make_fl_cell(void* addr, size_t size) {
    fl_cell* flc = addr;
    flc->addr = addr;
    flc->size = size;
    flc->next = NULL;

    return flc;    
}

long list_chunks(fl_cell* start, long count) {
    
    count += (long) start->size;
    
    if (start->next) {
        count += list_chunks(start->next, count);
    }

    return count;
}

long free_list_length() {
    // TO-DONE?: Calculate the length of the free list.
    long pages = stats.pages_mapped * PAGE_SIZE;
    long chunks = list_chunks(&head, 0);
    
    return chunks - pages;
}

hm_stats* hgetstats() {
    stats.free_length = free_list_length();
    return &stats;
}

void hprintstats() {
    stats.free_length = free_list_length();
    fprintf(stderr, "\n== husky malloc stats ==\n");
    fprintf(stderr, "Mapped:   %ld\n", stats.pages_mapped);
    fprintf(stderr, "Unmapped: %ld\n", stats.pages_unmapped);
    fprintf(stderr, "Allocs:   %ld\n", stats.chunks_allocated);
    fprintf(stderr, "Frees:    %ld\n", stats.chunks_freed);
    fprintf(stderr, "Freelen:  %ld\n", stats.free_length);
}

static size_t div_up(size_t xx, size_t yy) {
    // This is useful to calculate # of pages
    // for large allocations.
    size_t zz = xx / yy;

    if (zz * yy == xx) {
        return zz;
    }
    else {
        return zz + 1;
    }
}

void* find_last(fl_cell* cell) {
    
    if (cell->next) {
        return find_last(cell->next);
    }
    else {
        return cell->addr;
    }

}

fl_cell* search_size(fl_cell* cell, size_t size) {

    if (cell->size >= size) {
        return cell->addr;
    }
    else {
        if (cell->next) {
            return search_size(cell->next, size);
        }
        else {
            return NULL;
        }
    }
}

void divvy_up(fl_cell* cell, size_t size) {
    
    size_t leftover = cell->size - size;
    fl_cell* og_next = cell->next;

    fl_cell second_half;
    fl_cell* new_cell = make_fl_cell(&second_half, leftover);

    new_cell->next = og_next;

    cell->size = size;
    cell->next = new_cell;
    
}


void* hmalloc(size_t size) {
    stats.chunks_allocated += 1;
    size += sizeof(size_t);

    // TODO: Check if there is a big enough block on the free list
    //       If not, mmap new block (1 page = 4096)
    //       If the block found is larger than the request AND the 
    //       leftover is large enough for a free list cell, return 
    //       the extra to the free list
    //       Use the start of the block to store its size
    //       return a pointer to the block that is *after* the size field
    
    fl_cell* any_free = search_size(&head, size);

    if (any_free) {
        size_t difference = any_free->size - size;
        
        if (difference > sizeof(fl_cell)) {
            divvy_up(any_free, size);
            // return &any_free->size + sizeof(size_t);??
        }
        else {
          // Give them the whole block
          
          // The clause basically ends here
          // return void* addr;
          // but how do i make sure im not giving them the addr of the cell
          // they should only be getting the size
          // So do i do do this?:
          // return &any_free->size + sizeof(size_t);

        }
    }
    else {
        
        // If there aren't any chunks large enough

        int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;

        void* new_page = mmap(NULL, 4096, prot, flags, -1, 0);

        fl_cell* last_cell = find_last(&head);
        
        fl_cell* new_cell = make_fl_cell(new_page, 4096);
        
        last_cell->next = new_cell;

        return hmalloc(size);
    }
        
    return (void*) 0xDEADBEEF;
}

void consolidate(fl_cell* cell) {
    
    // Go through the free list and figure out if any pointers are close?
    // Requires pointer arithmetic to count up the bytes of size, right?
    // How does my fl_cell struct fit in with the actual usable memory?

}

void hfree(void* item) {

    // Find the info about the given chunk (size, anything else?)
    // Maybe initialize all that to 0?
    // Place cell into the free list
    

    
    // What is this stats thing for??
    stats.chunks_freed += 1;

    // consolidate(&head);
}

