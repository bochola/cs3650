#ifndef INODE_H
#define INODE_H

#include "pages.h"

typedef struct inode {
    int mode; // permission & type; zero for unused
              // 1 for reserved, 1 < mode < 0xFFFF otherwise
    int size; // bytes
    // inode #x always uses data page #x
} inode;

void print_inode(inode* node);
inode* get_inode(int inum);
int alloc_inode();
void free_inode();
int inode_get_pnum(inode* node, int fpn);

#endif
