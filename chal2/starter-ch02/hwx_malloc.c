// hmalloc.c

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

typedef struct arena {
    fl_cell* head;
    pthread_mutex_t fl_lock;
} arena;

static arena* a[16];

const size_t PAGE_SIZE = 4096;
static hm_stats stats;
static __thread fl_cell* head;
static __thread pthread_mutex_t fl_lock;

int
get_arena() {
	for(int i =0; i < 16; i++) {
	    pthread_mutex_t temp = a[i]->fl_lock;
	    int pt = pthread_mutex_trylock(&temp);
	    if(pt == 0) {
	       head = a[i]->head;
	       fl_lock = a[i]->fl_lock;
	       return i;
	    }
	    else if(pt == atol("ENIVAL")) {
           	pthread_mutex_init(&temp, 0);
	    }
        }
        return get_arena();
}

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
	printf("chunk of memory big enough\n");
        return 0;
    }
    assert(cell->size);
    printf("size = %ld\ncell->size = %ld\n", size, cell->size);
    if (cell->size >= size) {
	
        return cell;
    }

    return search_size(cell->next, size);
}

void split_chunk(fl_cell* cell, size_t size) {
    printf("Split Chunk: size = %d\n", (int)size);
    size_t leftover = cell->size - size;
    assert(leftover > sizeof(fl_cell));
    cell->size = size;
    
    fl_cell* second_half = (fl_cell*) (((char*) cell) + size);
    fl_cell* new_cell = make_fl_cell(second_half, leftover);
    insert_fl_cell(new_cell);
}


void* xmalloc(size_t size) {
    //pthread_key_t hkey;
    //pthread_key_create(&hkey, (void*)head);
    //pthread_key_t pkey;
    //pthread_key_create(&pkey, (void*)&fl_lock);
    //int i = get_arena();
    //head = a[i]->head;
    //fl_lock = a[i]->fl_lock;
    pthread_mutex_lock(&fl_lock);
    size += sizeof(size_t);
    printf("Size + size_t = %ld\n", size);

    if (size < sizeof(fl_cell)) {
        size = sizeof(fl_cell);
    }

    if (size >= PAGE_SIZE) {
        size_t num_mmap = div_up(size);
        printf("pages = %ld\n", num_mmap);
        int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;

        void* new_page = mmap(NULL, num_mmap * PAGE_SIZE, prot, flags, -1, 0);
        stats.pages_mapped += num_mmap;
        
        size_t* add_size = new_page;
        *add_size = num_mmap * PAGE_SIZE;

        stats.chunks_allocated += 1;
        //pthread_key_delete(hkey);
        //pthread_key_delete(pkey);
        pthread_mutex_unlock(&fl_lock);
        return add_size + 1;
    }
    
    
    printf("Start search ...\n");
    fl_cell* any_free = search_size(head, size);
    printf("finish search\n");

    if (any_free) {
	printf("any_free is not null\n");
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

        if (head == any_free) {
            head = next;
        }

        stats.chunks_allocated += 1;
        pthread_mutex_unlock(&fl_lock);
        //pthread_key_delete(hkey);
        //pthread_key_delete(pkey);
        return address;
    }
    else {
        printf("any_free is null\n");
        // If there aren't any chunks large enough

        int prot = PROT_EXEC | PROT_READ | PROT_WRITE;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;

        void* new_page = mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);
        stats.pages_mapped += 1;

        fl_cell* new_cell = make_fl_cell(new_page, PAGE_SIZE);
        insert_fl_cell(new_cell);
        
        coalesce(new_cell);
        pthread_mutex_unlock(&fl_lock);
        //pthread_key_delete(hkey);
        //pthread_key_delete(pkey);
        return xmalloc(size - sizeof(size_t));
    }
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


void xfree_helper(void* item) {
    size_t* size = ((size_t*) item) - 1;
    
    if (*size >= PAGE_SIZE) {
        munmap(item, div_up(*size) * PAGE_SIZE);
        stats.pages_unmapped += div_up(*size);
        stats.chunks_freed += 1;
    }
    else {
        fl_cell* returned = make_fl_cell(size, *size);
        insert_fl_cell(returned);
        coalesce(returned);
        stats.chunks_freed += 1;
    }

}

void xfree(void* item) {
    //pthread_key_t hkey;
    //pthread_key_create(&hkey, (void*)head);
    //pthread_key_t pkey;
    //pthread_key_create(&pkey, (void*)&fl_lock);
    //int i = get_arena();
    //head = a[i]->head;
    //fl_lock = a[i]->fl_lock;
    pthread_mutex_lock(&fl_lock);
    if(item) {
        xfree_helper(item);
    }
    pthread_mutex_unlock(&fl_lock);
    //pthread_key_delete(hkey);
    //pthread_key_delete(pkey);
    
}

void* xrealloc(void* item, size_t new_size) {
    
    // Find a chunk that is large enough for item size + n
    /*
     * The realloc() function changes the size of the memory block 
     * pointed to by ptr to size bytes.  The contents will be 
     * unchanged in the range from the start of the region up  to  
     * the minimum  of  the  old  and new sizes.  If the new size is 
     * larger than the old size, the added memory will not be 
     * initialized.  If ptr is NULL, then the call is equivalent to 
     * malloc(size), for all values of size; if size is equal to 
     * zero, and ptr is not NULL, then the call is equivalent to 
     * free(ptr).  Unless ptr is NULL, it must have been returned by
     * an earlier call to malloc(), calloc(), or realloc().  If the 
     * area pointed to was moved, a free(ptr) is done.
     */
	pthread_mutex_lock(&fl_lock);
	if(!item && new_size) {
                pthread_mutex_unlock(&fl_lock);
		printf("item null\n");
		return xmalloc(new_size);
	}
	if(!item && !new_size) {
		printf("item and size null\n");
		return NULL;
	}
	if(new_size == 0) {
                printf("Size is 0, return null\n");
		pthread_mutex_unlock(&fl_lock);
		xfree(item);
		return NULL; // ??????
	}
	

	size_t* item_size = (size_t*)item - 1;
	printf("item location= %p\n", item);

	if(*item_size < new_size) {
                pthread_mutex_unlock(&fl_lock);
		void* new_item = xmalloc(*item_size);
		pthread_mutex_lock(&fl_lock);
		memcpy(new_item, item, *item_size);
		printf("Call if xfree\n");
		pthread_mutex_unlock(&fl_lock);
		xfree(item);
		printf("Finished if Free new_item=%p\n", new_item);
		return new_item;
	}
	else {
		pthread_mutex_unlock(&fl_lock);
		void* new_item = xmalloc(new_size);
		pthread_mutex_lock(&fl_lock);
		memcpy(new_item, item, new_size);
		printf("Call free else \n");
		pthread_mutex_unlock(&fl_lock);
		xfree(item);
		printf("finished else free\n");
		return new_item;
	}
}
