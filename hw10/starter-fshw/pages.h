//pages.h

#ifndef PAGES_H
#define PAGES_H

#include <stdio.h>

void pages_init(const char* path, int create);
void pages_free();
void* pages_get_page(int pnum);

// Copied from hints
void* get_pages_bitmap();
void* get_inode_bitmap();
void* get_inode_base();
int alloc_page();
void free_page(int pnum);

#endif
