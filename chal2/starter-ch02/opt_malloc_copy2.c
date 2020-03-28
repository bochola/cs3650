// This is a straight-up copy of hwx_malloc.c
// I couldn't get opt_malloc to work, but the work for it is in
// opt_malloc_copy. This file was copied to see if my hwx file was 
// good enough to pass the opt tests, which it is for all but two
// tests. I've been working on this for almost 29 hours now so
// I'm tired

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
static fl_cell* head;
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
        printf("Last cell was 0x%p. ", cell->last);

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
    
    if (!cell) {
        return 0;
    }
    else {
        return 1 + fl_length(cell->next);
    }
}

long free_list_length() {
    return fl_length(head);
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

void insert_fl_cell(fl_cell* cell) {

    if (!head) {
        cell->last = NULL;
        cell->next = NULL;
        head = cell;
        return;
    }

    for (fl_cell* cur = head; cur; cur = cur->next) {
        if (cur > cell) {
            
            if (cur->last) {
                cur->last->next = cell;
                cell->last = cur->last;
            }
            else {
                cell->last = NULL;
                head = cell;
            }
            
            cell->next = cur;
            cur->last = cell;
            break;
        }
        else if (!cur->next) {
            cur->next = cell;
            cell->last = cur;
            cell->next = NULL;
            break;
        }
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
    assert(cell->size);
    if (cell->size >= size) {
	
        return cell;
    }

    return search_size(cell->next, size);
}

void split_chunk(fl_cell* cell, size_t size) {
    size_t leftover = cell->size - size;
    assert(leftover > sizeof(fl_cell));
    cell->size = size;
    
    fl_cell* second_half = (fl_cell*) (((char*) cell) + size);
    fl_cell* new_cell = make_fl_cell(second_half, leftover);
    insert_fl_cell(new_cell);
}


void coalesce(fl_cell* cell) {
    char* cell_addr = (char*) cell;
    char* cell_end = ((char*) cell) + cell->size;

    if (cell->next) {
        
        fl_cell* next = cell->next;
        char* next_addr = (char*) next;

        if (cell_end == next_addr) {
            cell->next = next->next;
            cell->size += next->size;
            
            if (next->next) {
                next->next->last = cell;
            }
        }
        else if ((next_addr < cell_end) && (next > cell)) {
            printf("Unreachable, next cell cant be located in current\n");
            assert(0);
        }
    }

    if (cell->last) {

        fl_cell* last = cell->last;
        char* last_end = ((char*) last) + last->size;

        if (last_end == cell_addr) {
            last->next = cell->next;
            last->size += cell->size;

            if (cell->next) {
                cell->next->last = last;
            }
        }
        else if ((cell_addr < last_end) && (cell > last)) {
            printf("Unreachable, current cell cant be located in last\n");
            assert(0);
        }
    }
}

void* xmalloc_helper(size_t size) {
    
    if (size < sizeof(fl_cell)) {
        size = sizeof(fl_cell);
    }

    if (size >= PAGE_SIZE) {
        size_t num_pages = div_up(size);

        void* new_page = mmap(NULL, num_pages * PAGE_SIZE, prot, flags, -1, 0);
        
        size_t* add_size = new_page;
        *add_size = num_pages * PAGE_SIZE;

        return add_size + 1;
    }
    
    fl_cell* any_free = search_size(head, size);

    if (!any_free) {
        void* new_page = mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);

        fl_cell* new_cell = make_fl_cell(new_page, PAGE_SIZE);
        insert_fl_cell(new_cell);
        
        coalesce(new_cell);
        return xmalloc_helper(size);
    }

    size_t difference = any_free->size - size;
        
    if (difference > sizeof(fl_cell)) {
        split_chunk(any_free, size);
    }
     
    fl_cell* last = any_free->last;
    fl_cell* next = any_free->next;

    if (last) {
        last->next = next;
    }
    else {
        head = next;
    }
    
    if (next) {
        next->last = last;
    }

    return ((size_t*) any_free) + 1;
}

void* xmalloc(size_t size) {

    pthread_mutex_lock(&fl_lock);
    void* ptr = xmalloc_helper(size + sizeof(size_t));
    pthread_mutex_unlock(&fl_lock);

    return ptr;
}



void xfree_helper(void* item) {
    fl_cell* cell_addr = (fl_cell*)(((size_t*) item) - 1);
    size_t size = cell_addr->size;
    
    if (size >= PAGE_SIZE) {
        munmap(cell_addr, div_up(size) * PAGE_SIZE);
    }
    else {
        fl_cell* returned = make_fl_cell(cell_addr, size);
        insert_fl_cell(returned);
        coalesce(returned);
    }

}

void xfree(void* item) {
    pthread_mutex_lock(&fl_lock);
    assert(item);
    xfree_helper(item);
    pthread_mutex_unlock(&fl_lock);
}

void* xrealloc_helper(void* item, size_t new_size) {
    
	if(!item && new_size) {
		return xmalloc_helper(new_size + sizeof(size_t));
	}
	if(!item && !new_size) {
		return NULL;
	}
	if(new_size == 0) {
		xfree_helper(item);
		return NULL; // ??????
	}

    fl_cell* cell_addr = (fl_cell*)(((size_t*) item) - 1);
    size_t size = cell_addr->size;

    void* new_item = xmalloc_helper(new_size + sizeof(size_t));

	if(size < new_size) {
		memcpy(new_item, item, size);
	}
	else {
		memcpy(new_item, item, new_size);
	}

    xfree_helper(item);
	return new_item;

}

void* xrealloc(void* item, size_t new_size) {

	pthread_mutex_lock(&fl_lock);
    void* ptr = xrealloc_helper(item, new_size);
    pthread_mutex_unlock(&fl_lock);

    return ptr;
}

