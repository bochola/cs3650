#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "xmalloc.h"

/*
  // Free list cell
  // min allocation is a user request of 8bytes, so anything smaller
  // needs to be rounded up
  typedef struct fl_cell {
    size_t size;
    fl_cell* next;
  } fl_cell;
*/

const int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
const int flags = MAP_PRIVATE | MAP_ANONYMOUS;
const size_t PAGE_SIZE = 4096;
const int HEAP_SIZE = 1048576;
static fl_cell* heads[16]; 
// head[0] starts a fl_cell chain of sizes 2^(0+5) = 32 bytes
static pthread_mutex_t fl_lock = PTHREAD_MUTEX_INITIALIZER;

fl_cell* make_fl_cell(void* addr, size_t size) {
    fl_cell* flc = addr;
    flc->size = size;
    flc->last = NULL;
    flc->next = NULL;

    return flc;    
}

void print_fl(fl_cell* cell) {
    
    if (cell) {
        printf("This cell is %ld bytes at 0x%p. ", cell->size, cell);

        if (cell->next) {
            printf("Next cell is 0x%p. ", cell->next);
            printf("\n");
            print_fl(cell->next);
        }
        else {
            printf("Without a next. ");
        }

        printf("\n");
    }
    else {
        printf("No cell.\n");
    }
}

long fl_length(fl_cell* cell) {
    
    printf("Entering fl_length\n");
    if (!cell) {
        return 0;
    }
    else {
        return 1 + fl_length(cell->next);
    }
}

void print_headcount() {
    long x;
    //printf("\n");
    for (int i = 0; i < 16; i++) {
        //printf("Before fl_length test\n");
        long ans = fl_length(heads[i]);
        x += ans;
        //printf("fl_length works\n");
        //printf("head[%d] count = %ld\n", i, ans);
    }
    //printf("\n");
}

// Returns the head of the fl at heads[index]
// and repairs the link
fl_cell* pop(int index) {
    fl_cell* temp = heads[index];
    printf("Popping index %d\n", index);
    heads[index] = heads[index]->next;
    return temp;
}

void insert_fl_cell(int i, fl_cell* cell) {
    cell->next = heads[i];
    heads[i] = cell;
}

void pull_from_above(int i) {
    
    fl_cell* snatched = pop(i + 1);
    
    size_t size = (size_t) pow(2, i + 5);
    snatched->size = size;
    
    void* new_cell_ptr = ((char*) snatched) + size;
    fl_cell* new_cell = make_fl_cell(new_cell_ptr, size);

    insert_fl_cell(i, new_cell);
    insert_fl_cell(i, snatched);
}

void redistribute(int i) {
    printf("Redistributing for %d...\n", i);
    print_headcount();
    // Given an index i, redistribute should:
    //      1. Check if heads[i] is empty
    //          a. If it isnt empty, return
    //          b. If it is empty, continue
    //      2. Try to grab a cell from i + 1
    //          a. Split that cell in half and add both cells to 
    //             heads[i]
    //      3. If the free list at array i + 1 is also empty, then
    //         do the same for i + 2 and cascade down until back at
    //         i
    //      4. If the last free list (heads[15]) is empty, then mmap
    //         more space and add that to heads[15].
    //
    if (heads[i]) {
        print_headcount();
        printf("Sucessful redistribution for %d\n\n", i);
        return;
    }
    
    if (i == 15) {
        void* new_cell = mmap(NULL, HEAP_SIZE, prot, flags, -1, 0);
        heads[15] = make_fl_cell(new_cell, HEAP_SIZE);
        return;
    }
    redistribute(i + 1);
    printf("Pulling from above for %d\n", i);
    print_headcount();
    pull_from_above(i);
    printf("Yanked down 2 cells for %d\n", i);
    print_headcount();

    print_headcount();
    printf("Sucessful redistribution for %d\n\n", i);
}

int get_fl_index(size_t size) {

    for(int i = 0; i < 16; i++) {
        if(pow(2, i + 5) > size) {
	        return i;
        }
    }
    assert(0); // Shouldnt ever get here
}

void* xmalloc_helper(size_t size) {
    
    print_headcount();
    printf("\nEntering malloc\n"); 
    if (size < sizeof(fl_cell)) {
        size = sizeof(fl_cell);
    }

    if (size > HEAP_SIZE) {
        void* new_page = mmap(NULL, size, prot, flags, -1, 0);
        
        size_t* add_size = new_page;
        *add_size = size;

        print_headcount();
        printf("Leaving malloc with mmap\n");
        return add_size + 1;
    }
    
    int fl_index = get_fl_index(size);
    //int power = fl_index + 5;
    
    print_headcount();
    printf("Redistributing for %d from malloc\n", fl_index);
    redistribute(fl_index);
    fl_cell* designated = pop(fl_index);

    if (!designated) {
        assert(0); // Shouldnt ever get here
                   // assign_fl should create cells of the correct
                   // size
    }
    print_headcount();
    printf("Leaving malloc\n");
    return ((size_t*) designated) + 1;
}

void* xmalloc(size_t size) {

    pthread_mutex_lock(&fl_lock);
    void* ptr = xmalloc_helper(size + sizeof(size_t));
    pthread_mutex_unlock(&fl_lock);

    return ptr;
}

void xfree_helper(void* item) {

    
    print_headcount();
    printf("Entering free\n");
    fl_cell* cell_addr = (fl_cell*)(((size_t*) item) - 1);
    size_t size = cell_addr->size;
    
    if (size > HEAP_SIZE) {
        munmap(cell_addr, size);
        printf("Munmap complete\n");
    }
    else {
        
        int fl_index = get_fl_index(size);
        insert_fl_cell(fl_index, cell_addr);
        printf("Free complete\n");
    }
}

void xfree(void* item) {
    pthread_mutex_lock(&fl_lock);
    xfree_helper(item);
    pthread_mutex_unlock(&fl_lock);
}

void* xrealloc_helper(void* item, size_t new_size) {
    
    print_headcount();
    printf("Inside realloc\n");
	if(!item) {
		return xmalloc_helper(new_size + sizeof(size_t));
	}
	if(new_size == 0) {
		xfree_helper(item);
        printf("Leaving realloc\n");
		return NULL;
	}

    new_size += sizeof(size_t);

    fl_cell* cell_addr = (fl_cell*)(((size_t*) item) - 1);
    size_t size = cell_addr->size;
    
    if (new_size <= size) {
        printf("Leaving realloc\n");
        return item;
    }
    
    print_headcount();
    void* new_item = xmalloc_helper(new_size);
    print_headcount();
    printf("Good xmalloc\n");
    memcpy(new_item, item, size - sizeof(size_t));
    print_headcount();
    printf("Good memcpy\n");
    xfree_helper(item);
    print_headcount();
    printf("Good free, leaving realloc\n");

	return new_item;
}

void* xrealloc(void* item, size_t new_size) {
    
	pthread_mutex_lock(&fl_lock);
    void* ptr = xrealloc_helper(item, new_size);
    pthread_mutex_unlock(&fl_lock);

    return ptr;
}
