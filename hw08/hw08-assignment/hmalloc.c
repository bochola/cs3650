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
    size_t size;
    fl_cell* next;
  } fl_cell;
*/

const size_t PAGE_SIZE = 4096;
static hm_stats stats; // This initializes the stats to 0.
static fl_cell* head; // I dont want to have to initialize head every single time...
                     // but how do i make sure that the entire free list is after it?

fl_cell* make_fl_cell(void* addr, size_t size, fl_cell* last) {
    fl_cell* flc = addr;
    flc->size = size;
    flc->last = last;
    flc->next = NULL;

    return flc;    
}

void print_fl(fl_cell* cell) {
    
    if (cell) {
        printf("This cell is %ld bytes at 0x%x. ", cell->size, cell);
        printf("Last cell was 0x%x. ", cell->last);

        if (cell->next) {
            printf("Next cell is 0x%x.", cell->next);
            printf("\n");
            print_fl(cell->next);
        }
        else {
            printf("Without a next.");
        }

        printf("\n");
    }
    else {
        printf("No cell.\n");
    }
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
    long chunks = list_chunks(head, 0);
    
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
    size_t zz = xx / PAGE_SIZE;

    if (zz * PAGE_SIZE == xx) {
        return zz;
    }
    else {
        return zz + 1;
    }
}

void* find_last(fl_cell* cell) {
    if (!cell) {
        return 0;
    }

    if (cell->next) {
        return find_last(cell->next);
    }
    else {
        return cell;
    }

}

fl_cell* search_size(fl_cell* cell, size_t size) {
    
    if (!cell) {
        return 0;
    }

    if (cell->size >= size) {
        return cell;
    }

    return search_size(cell->next, size);
}

void divvy_up(fl_cell* cell, size_t size) {
   
    size_t leftover = cell->size - size;
    fl_cell* og_next = cell->next;

    fl_cell* second_half = (fl_cell*) (((char*) cell) + size);
    fl_cell* new_cell = make_fl_cell(second_half, leftover, cell);

    new_cell->next = og_next;

    cell->size = size;
    cell->next = new_cell;
    printf("\n After divvy_up:\n");
    print_fl(head);
}


void* hmalloc(size_t size) {
    size += sizeof(size_t);
    print_fl(head);
    printf("malloc-ing %ld bytes\n", size);

    if (size >= PAGE_SIZE) {
        size_t num_mmap = div_up(size);
        
        int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;

        void* new_page = mmap(NULL, num_mmap * PAGE_SIZE, prot, flags, -1, 0);

        size_t* add_size = new_page;
        *add_size = num_mmap * PAGE_SIZE;

        return add_size + 1;
    }
    
    
    fl_cell* any_free = search_size(head, size);

    if (any_free) {
        printf("Found a suitable chunk\n");

        size_t difference = any_free->size - size;
        
        if (difference > sizeof(fl_cell)) {
            printf("Chunk large enough to split\n");
            divvy_up(any_free, size);
        }
         
        size_t* address = (size_t*) any_free;
        address += 1;
        printf("After divvy up\n");  
        fl_cell* last = any_free->last;
        fl_cell* next = any_free->next;
        printf("Last is at 0x%x\nNext is at 0x%x\n", last, next);
        printf("After last and next assignment\n");
        if (last != NULL) {
            printf("Before line 195. Last is 0x%x\n", last);
            last->next = next;
            printf("After line 195\n");
        }
        
        if (next) {
            printf("Before line 199\n");
            next->last = last;
            printf("After line 199\n");
        }
        printf("After if(last) and if(next)\n");
        if (head == any_free) {
            head = next;
        }

        stats.chunks_allocated += 1;
        print_fl(head);
        return address;
    }
    else {
        
        // If there aren't any chunks large enough

        printf("No chunks large enough, adding page\n");
        int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;

        void* new_page = mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);

        printf("Page added, updating free list\n");
        
        fl_cell* last_cell = find_last(head);
        
        fl_cell* new_cell = make_fl_cell(new_page, PAGE_SIZE, last_cell);
        
        if (last_cell) {
            last_cell->next = new_cell;
        }
        else {
            printf("Updated head\n");
            head = new_cell;
        }

        return hmalloc(size - sizeof(size_t));
    }
}

void consolidate(fl_cell* cell) {
    
    // Go through the free list and figure out if any pointers are close?
    // Cast to char*, add size, check if equal to next cell address

}

void hfree(void* item) {

    printf("Entered hfree\n");
    
    print_fl(head);

    size_t* size = (size_t*)item - 1;
    printf("Freeing %ld bytes\n", *size);
    
    if (*size >= PAGE_SIZE) {
        munmap(item, div_up(*size) * PAGE_SIZE);
        stats.pages_unmapped += div_up(*size);
    }
    else {
        printf("Smaller free\n");
        fl_cell* last_cell = find_last(head);
        printf("Found last cell\n");
        fl_cell* returned = make_fl_cell(size, *size, last_cell);
        printf("Added cell back to list\n");
        if (last_cell) {
            last_cell->next = returned;
        }
        printf("Finished\n");
        stats.chunks_freed += 1;
    }

    // consolidate(head);
}

