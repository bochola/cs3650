// directory.h

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <bsd/string.h>

#include "slist.h"
#include "pages.h"
#include "inode.h"

typedef struct dirent {
    char name[60];
    int  inum;
} dirent;

int directory_init();
char* directory_get(inode* dir);
int directory_lookup(inode* dir, const char* name);
int tree_lookup(const char* path);
int directory_put(inode* dir, const char* name, int inum);
int directory_delete(inode* dir, const char* name);
slist* directory_list(inode* dir);
void print_directory(inode* dir);

#endif

