
#define _GNU_SOURCE
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include "directory.h"
#include "pages.h"
#include "slist.h"
#include "util.h"
#include "inode.h"

#define ENT_SIZE 16

int
directory_init()
{
    int inum = alloc_inode();
    inode* rn = get_inode(inum);
    rn->mode = 040755;
    grow_inode(rn, 4096);
    printf("Directory inum: %d\n", inum);
    return inum;   
}

int
directory_lookup(inode* dir, const char* name)
{
    dirent* start = pages_get_page(inode_get_pnum(dir, 0));
    
    for (int i = 0; i < 64; i++) {
        char* ent = start[i].name;
        if (streq(ent, name)) {
            return start[i].inum;
        }
    }
    return -ENOENT;
}

int
tree_lookup(const char* path)
{
    printf("Path: %s\n", path);
    assert(path[0] == '/');

    slist* s_path = s_split(path, '/');
    
    int cur_inum = 0;
    for (slist* cur = s_path; cur; cur = cur->next) {
        char* base = cur->data;
        inode* cur_dir = get_inode(cur_inum);
        cur_inum = directory_lookup(cur_dir, base);

        if (cur_inum < 0) {
            return cur_inum;
        }
    }
    
    return cur_inum;
}

int
directory_put(inode* dir, const char* name, int inum)
{
    dirent* start = pages_get_page(inode_get_pnum(dir, 0));
    
    int check = directory_lookup(dir, name);
    if (check > 0) {
        printf("Destination already exists, please fix\n");
        return -1;
    }
    if (strlen(name) > 60) {
        printf("Destination name is too long\n");
        return -1;
    }    

    int empty_index;
    for (int i = 0; i < 64; i++) {
        if (start[i].name[0] == 0) {
            empty_index = i;
            break;
        }

        if (i == 63) {
            return -1;
        }
    }
    
    dirent* new_ent = start + empty_index;
    strncpy(new_ent->name, name, 59);
    new_ent->name[59] = '\0';
    new_ent->inum = inum;

    inode* og = get_inode(inum);
    og->refs++;
    
    printf("+ directory_put(..., %s, %d) -> 0\n", name, inum);
    print_inode(og);

    return 0;
}

int
directory_delete(inode* dir, const char* name)
{
    printf(" + directory_delete(%s)\n", name);

    dirent* start = pages_get_page(inode_get_pnum(dir, 0));
    
    int dir_index;
    for (int i = 0; i < 64; i++) {
        if (streq(start[i].name, name)) {
            dir_index = i;
            break;
        }

        if (i == 63) {
            return -ENOENT;
        }
    }
    
    dirent* trashed = start + dir_index;
    trashed->name[0] = 0;
    trashed->inum = 0;
    
    free_inode(dir);
    return 0;
}

slist*
directory_list(inode* dir)
{
    printf("+ directory_list()\n");
    slist* ys = 0;

    dirent* start = pages_get_page(inode_get_pnum(dir, 0));
    
    for (int i = 0; i < 64; i++) {
        if (start[i].name[0] != 0) {
            ys = s_cons(start[i].name, ys);
        }
    }

    return ys;
}

void
print_directory(inode* dd)
{
    printf("Contents:\n");
    slist* items = directory_list(dd);
    for (slist* xs = items; xs != 0; xs = xs->next) {
        printf("- %s\n", xs->data);
    }
    printf("(end of contents)\n");
    s_free(items);
}
