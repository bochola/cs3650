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

fl_cell* make_fl_cell(void* addr, size_t size, fl_cell* last) {
    fl_cell* flc = addr;
    flc->addr = addr;
    flc->size = size;
    flc->next = NULL;
    flc->last = last;

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

static size_t div_up(size_t xx) {
    // This is useful to calculate # of pages
    // for large allocations.
    size_t zz = xx / 4096;

    if (zz * 4096 == xx) {
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
    fl_cell* new_cell = make_fl_cell(&second_half, leftover, cell);

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
    
    if (size > 4096) {
        size_t num_mmap = div_up(size);
        
        int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;

        void* new_page = mmap(NULL, num_mmap * 4096, prot, flags, -1, 0);

        size_t* add_size = new_page;
        *add_size = num_mmap * 4096;

        return add_size + 1;
    }
    
    
    fl_cell* any_free = search_size(&head, size);

    if (any_free) {
        size_t difference = any_free->size - size;
        
        if (difference > sizeof(fl_cell)) {
            divvy_up(any_free, size);
        }
          
        size_t* address = (size_t*) any_free;
        address += 1;
          
        fl_cell* last = any_free->last;
        fl_cell* next = any_free->next;

        last->next = next;
        next->last = last;
          
        return address;
    }
    else {
        
        // If there aren't any chunks large enough

        int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;

        void* new_page = mmap(NULL, 4096, prot, flags, -1, 0);

        fl_cell* last_cell = find_last(&head);
        
        fl_cell* new_cell = make_fl_cell(new_page, 4096, last_cell);
        
        last_cell->next = new_cell;

        return hmalloc(size);
    }
}

void consolidate(fl_cell* cell) {
    
    // Go through the free list and figure out if any pointers are close?
    // Cast to char*, add size, check if equal to next cell address

}

void hfree(void* item) {

    size_t* size = (size_t*)item - 1;

    if (*size > 4096) {
        munmap(item, *size);
        stats.pages_unmapped += *size / 4096;
    }
    else {
        fl_cell* last_cell = find_last(&head);
        fl_cell* returned = make_fl_cell((void*) size, *size, last_cell);
        
        last_cell->next = returned;

        stats.chunks_freed += 1;
    }

    // consolidate(&head);
}

