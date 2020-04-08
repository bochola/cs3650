//inode.c

#include <stdint.h>
#include <assert.h>

#include "bitmap.h"
#include "pages.h"
#include "inode.h"
#include "util.h"

const int INODE_COUNT = 256;

inode* get_inode(int inum) {
    
    inode* nodes = get_inode_base();
    return &(nodes[inum]);
}

int get_inode_num(void* ptr) {
    
}

int alloc_inode() {

    for (int ii = 0; ii < INODE_COUNT; ++ii) {
        
        int val = bitmap_get(get_inode_bitmap(), ii);
        inode* node = get_inode(ii);
        
        if (val == 0) {
            memset(node, 0, sizeof(inode));
            node->mode = 0100644; 
            bitmap_set(get_inode_bitmap(), ii, 1);
            printf("+ alloc_inode() -> %d\n", ii);
            return ii;
        }
    }

    return -1;
}

void free_inode(int inum) {

    printf("+ free_inode(%d)\n", inum);
    
    if (inum < 0) {
        printf("Invalid inode number. inode.c: 47\n");
        return;
    }

    inode* node = get_inode(inum);
    node->refs--;
    
    if (node->refs == 0) {
        
        shrink_inode(node, 0);
        memset(node, 0, sizeof(inode));
    
        void* ibm = get_inode_bitmap();

        bitmap_set(ibm, inum, 0);
    }
}

void
print_inode(inode* node)
{
    if (node) {
        printf("node{mode: %04o, size: %d}\n",
               node->mode, node->size);
    }
    else {
        printf("node{null}\n");
    }
}

int inode_get_pnum(inode* node, int fpn) {
    
    assert(fpn >= 0);
    
    if (fpn < 2) {
        return node->dptrs[fpn];
    }
    
    int* pg_nums = pages_get_page(node->iptr);
    return pg_nums[fpn - 2];
}

int pages_owned(inode* node) {
    
    int ans = 0;

    for (int i = 0; i < 1026; i++) {
        if (inode_get_pnum(node, i) > 0) {
            ans++;
        }
        else {
            break;
        }
    }

    return ans;
}

int grow_inode(inode* node, int size) {
    
    if (size > 1026 * 4096) {
        printf("Size too large. Cannot grow inode past 1026 pages\n");
        return -1;
    }
    
    for (int i = 0; i < 2; i++) {
        if ((pages_owned(node) * 4096 < size) && (node->dptrs[i] <= 0)) {
            int page_num = alloc_page();
            
            if (page_num < 0) {
                printf("Couldn't find a page to allocate. inode.c: 112\n");
                return page_num;
            }
            
            node->dptrs[i] = page_num;
        }

        if (pages_owned(node) * 4096 >= size) {
            return 0;
        }
    }

    if (!node->iptr) {
        int page_num = alloc_page();
        
        if (page_num < 0) {
            printf("Couldn't find a page to allocate. inode.c: 128\n");
            return page_num;
        }
            
        node->iptr = page_num;
    }
    
    int* page_nums = pages_get_page(node->iptr);

    for (int i = 0; i < 1024; i++) {
        if (pages_owned(node) * 4096 < size) {
            int added_page = alloc_page();
            
            if (added_page < 0) {
                printf("Couldn't find a page to allocate. inode.c: 142\n");
                return added_page;
            }
            
            page_nums[i] = added_page;
        } 

        if (pages_owned(node) * 4096 >= size) {
            return 0;
        }
    }
    
}

int shrink_inode(inode* node, int size) {
    
    if (size < 0) {
        printf("Bad input, inode size cannot be less than 0\n");
        return -1;
    }
    

    for (int i = pages_owned(node) - 1; i > 1; i--) {
        if (pages_owned(node) * 4096 >  size) {
            int* page_nums = pages_get_page(node->iptr);
            
            free_page(page_nums[pages_owned(node) - 2 - 1]);
            
            page_nums[pages_owned(node) - 2 - 1] = 0;
        }

        if (pages_owned(node) * 4096 <= size) {
            return 0;
        }

    }
    
    if (node->iptr) {
        free_page(node->iptr);
    }
    node->iptr = 0;
    
    for (int i = 1; i >= 0; i--) {
        if ((pages_owned(node) * 4096 >  size) && (node->dptrs[i] > 0)) {
            
            free_page(node->dptrs[i]);
            node->dptrs[i] = 0;
        }

        if (pages_owned(node) * 4096 <= size) {
            return 0;
        }
    }
}

