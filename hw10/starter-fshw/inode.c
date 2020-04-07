//inode.c

#include <stdint.h>

#include "pages.h"
#include "inode.h"
#include "util.h"

const int INODE_COUNT = 256;

inode*
get_inode(int inum)
{
    inode* nodes = get_inode_base();
    return &(nodes[inum]);
}

int
alloc_inode()
{
    for (int ii = 1; ii < INODE_COUNT; ++ii) {
        inode* node = get_inode(ii);
        if (node->mode == 0) {
            memset(node, 0, sizeof(inode));
            node->mode = 010644; 
            printf("+ alloc_inode() -> %d\n", ii);
            return ii;
        }
    }

    return -1;
}

void
free_inode(int inum)
{
    printf("+ free_inode(%d)\n", inum);

    inode* node = get_inode(inum);
    memset(node, 0, sizeof(inode));
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

