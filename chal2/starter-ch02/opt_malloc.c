#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "xmalloc.h"

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
const int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
const int flags = MAP_PRIVATE | MAP_ANONYMOUS;
const size_t PAGE_SIZE = 4096;
static hm_stats stats;
static __thread fl_cell* head[21];
const int HEAP_SIZE =  1048576;
void split_chunk(fl_cell* cell, int i);
static __thread pthread_mutex_t fl_lock = PTHREAD_MUTEX_INITIALIZER;
//head[20] = make_fl_cell(mmap(NULL, HEAP_SIZE, prot, flags, -1, 0), HEAP_SIZE);


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

void insert_fl_cell(int i, fl_cell* cell) {

    if (!head[i]) {
        cell->last = NULL;
        cell->next = NULL;
        head[i] = cell;
        return;
    }

    for (fl_cell* cur = head[i]; cur; cur = cur->next) {
        if (cur > cell) {
            
            if (cur->last) {
                cur->last->next = cell;
                cell->last = cur->last;
            }
            else {
                cell->last = NULL;
                head[i] = cell;
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

void break_up_chunk(int i) {
   if(i >= 20) {
       head[20] = mmap(NULL, HEAP_SIZE, prot, flags, -1, 0);
   }
   else if(head[i + 1]) {
       //break up
       fl_cell* temp = head[i + 1];
       head[i+1] = head[i+1]->next;
       split_chunk(temp, i);
       insert_fl_cell(i, temp);
   }
   else if(i+1 == 20) {
       head[20] = mmap(NULL, HEAP_SIZE, prot, flags, -1, 0);
       break_up_chunk(i);
   }
   else {
       break_up_chunk(i+1);
       break_up_chunk(i);
   }
}

int get_fl_index(size_t size) {

   for(int i = 6; i < 21; i++) {
	if(pow((double)2, (double)i) > size) {
	    return i;
        }
   }
   return -1;
}


fl_cell* search_size(fl_cell** cell, size_t size) {

   // find which freelist we are looking in
   int arr = get_fl_index(size);
   if (arr == -1) {
       return NULL;
    }
   if (head[arr] == NULL) {
       break_up_chunk(arr);
   }
   fl_cell* temp = head[arr];
   head[arr] = head[arr]->next;
   return temp;   
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


void split_chunk(fl_cell* cell, int i) {
    
    size_t size = (size_t)pow(2, i);
    size_t leftover = cell->size - size;
    cell->size = size;
    
    fl_cell* second_half = (fl_cell*) (((char*) cell) + size);
    fl_cell* new_cell = make_fl_cell(second_half, leftover);
    insert_fl_cell(i, new_cell);
}

void coalesce(fl_cell* cell) {
    
}

//MODIFY FOR BUDDY
void xfree_helper(void* item) {
    size_t* size = ((size_t*) item) - 1;
    
    if (*size >= PAGE_SIZE) {
        munmap(item, div_up(*size) * PAGE_SIZE);
    }
    else {
        fl_cell* returned = make_fl_cell(size, *size);
        insert_fl_cell(get_fl_index(*size), returned);
        coalesce(returned);
        stats.chunks_freed += 1;
    }

}

void*
xmalloc(size_t size)
{
    pthread_mutex_lock(&fl_lock);
    size += sizeof(size_t);
    
    if (size < sizeof(fl_cell)) {
        size = sizeof(fl_cell);
    }
    if (size >= HEAP_SIZE) {
        size_t num_mmap = div_up(size);
        
        

        void* new_page = mmap(NULL, num_mmap * PAGE_SIZE, prot, flags, -1, 0);
        stats.pages_mapped += num_mmap;
        
        size_t* add_size = new_page;
        *add_size = num_mmap * PAGE_SIZE;

        stats.chunks_allocated += 1;
        pthread_mutex_unlock(&fl_lock);
        return add_size + 1;
    }
    //pthread_mutex_lock(&fl_lock);
    fl_cell* any_free = search_size(head, size); 

    if (any_free) {

        size_t difference = any_free->size - size;
        
        if (difference > sizeof(fl_cell)) {
            split_chunk(any_free, size);
        }
         
        size_t* address = (size_t*) any_free;
        address += 1;
        fl_cell* last = any_free->last;
        fl_cell* next = any_free->next;

        if (last) {
            last->next = next;
        }
        
        if (next) {
            next->last = last;
        }

        if (head[get_fl_index(size)] == any_free) {
            head[get_fl_index(size)] = next;
        }

        stats.chunks_allocated += 1;
        pthread_mutex_unlock(&fl_lock);
        
        return address;
    }
    else {
        
        // If there aren't any chunks large enough

        int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;
        size_t num_mmap = div_up(size);

        void* new_page = mmap(NULL, PAGE_SIZE * num_mmap, prot, flags, -1, 0);
        stats.pages_mapped += 1;

        fl_cell* new_cell = make_fl_cell(new_page, PAGE_SIZE * num_mmap);
        insert_fl_cell(get_fl_index(PAGE_SIZE * num_mmap), new_cell);
        
        coalesce(new_cell);
        pthread_mutex_unlock(&fl_lock);
        
        return xmalloc(size - sizeof(size_t));
    }

    return 0;
}

void
xfree(void* item)
{
    pthread_mutex_lock(&fl_lock);
    xfree_helper(item);
    pthread_mutex_unlock(&fl_lock);
}

void*
xrealloc(void* item, size_t new_size)
{
    // TODO: write an optimized realloc
        if(new_size == 0) {
		xfree(item);
		printf("Size is 0, return null\n");
		return NULL; // ??????
	}

	size_t* item_size = (size_t*)item - 1;

	if(*item_size < new_size) {
		void* new_item = xmalloc(*item_size);
		memcpy(new_item, item, *item_size);
		xfree(item);
		return new_item;
	}
	else {
		void* new_item = xmalloc(new_size);
		memcpy(new_item, item, new_size);
		xfree(item);
		return new_item;
	}
    return 0;
}
