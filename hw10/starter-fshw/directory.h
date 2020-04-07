// directory.h

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <bsd/string.h>

#include "slist.h"
#include "pages.h"
#include "inode.h"

typedef struct dirent {
    char name[50];
    int  inum;
    char _reserved[12];
} dirent;

char* directory_get(int inum);
void directory_init();
int directory_lookup(const char* name);
int tree_lookup(const char* path);
int directory_put(const char* name, int inum);
int directory_delete(const char* name);
slist* directory_list();
void print_directory();

#endif

