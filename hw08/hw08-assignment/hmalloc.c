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
static fl_cell head;

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
    
    return pages + chunks;
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

    return (void*) 0xDEADBEEF;
}

void hfree(void* item) {
    
    // Find the info about the given chunk (size, anything else?)
    // Maybe initialize all that to 0?
    // Place cell into the free list

    stats.chunks_freed += 1;

}

