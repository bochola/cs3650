// inode.h

#ifndef INODE_H
#define INODE_H

#include "pages.h"

typedef struct inode {
    int refs; // how many things point to this inode
    int mode; // permission & type (directory or file) 
    int size; // bytes
    int dptrs[2]; // direct pointers
    int iptr; // indirect pointer
} inode;

void print_inode(inode* node);
inode* get_inode(int inum);
int alloc_inode();
void free_inode();
int inode_get_pnum(inode* node, int fpn);
int grow_inode(inode* node, int size); // copied from hints
int shrink_inode(inode* node, int fpn); // copied from hints

#endif
