//storage.c

#include <libgen.h>
//#undef basename      
#include <string.h>
// According to advice from stack overflow at
// https://stackoverflow.com/questions/5802191/
// Lines 3-5 get me the dirname and basename functions for 
// making nodes

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <alloca.h>
#include <bsd/string.h>
#include <stdint.h>

#include "storage.h"
#include "slist.h"
#include "util.h"
#include "pages.h"
#include "inode.h"
#include "directory.h"

void
storage_init(const char* path, int create)
{
    //printf("storage_init(%s, %d);\n", path, create);
    pages_init(path, create);
    if (create) {
        directory_init();
    }
}

int
storage_stat(const char* path, struct stat* st)
{
    printf("+ storage_stat(%s)\n", path);
    int inum = tree_lookup(path);
    if (inum < 0) {
        return inum;
    }

    inode* node = get_inode(inum);
    printf("+ storage_stat(%s); inode %d\n", path, inum);
    print_inode(node);

    memset(st, 0, sizeof(struct stat));
    st->st_uid   = getuid();
    st->st_mode  = node->mode;
    st->st_size  = node->size;
    st->st_nlink = 1;
    st->st_blocks = pages_owned(node) * 4;
    return 0;
}

int
storage_read(const char* path, char* buf, size_t size, off_t offset)
{
    int inum = tree_lookup(path);
    if (inum < 0) {
        return inum;
    }
    inode* node = get_inode(inum);
    printf("+ storage_read(%s); inode %d\n", path, inum);
    print_inode(node);

    if (offset >= node->size) {
        return 0;
    }

    if (offset + size >= node->size) {
        size = node->size - offset;
    }

    uint8_t* data = pages_get_page(inum);
    printf(" + reading from page: %d\n", inum);
    memcpy(buf, data + offset, size);

    return size;
}

int
storage_write(const char* path, const char* buf, size_t size, off_t offset)
{
    int trv = storage_truncate(path, offset + size);
    if (trv < 0) {
        return trv;
    }

    int inum = tree_lookup(path);
    if (inum < 0) {
        return inum;
    }

    inode* node = get_inode(inum);
    uint8_t* data = pages_get_page(inum);
    printf("+ writing to page: %d\n", inum);
    memcpy(data + offset, buf, size);

    return size;
}

int div_up(off_t size) {
    
    off_t nearest = size / 4096;

    if (nearest * 4096 == size) {
        return nearest;
    }
    else {
        return nearest + 1;
    }
}

int
storage_truncate(const char *path, off_t size)
{
    int inum = tree_lookup(path);
    if (inum < 0) {
        return inum;
    }

    int new_size = div_up(size);

    inode* node = get_inode(inum);
    int rv;
    if (node->size > size) {
        rv = shrink_inode(node, size);
    }
    else if (node->size < size) {
        rv = grow_inode(node, size);
    }
    
    node->size = size;
    return rv;
}

int
storage_mknod(const char* path, int mode)
{
    if (tree_lookup(path) != -ENOENT) {
        printf("mknod fail: already exist\n");
        return -EEXIST;
    }
    int inum;
    printf("Mode requested: %o", mode);
    int dir_bit = mode & 040000;
    if (dir_bit) {
        printf("Was directory bit, calling directory_init\n");
        inum = directory_init();
    }
    else {
        inum = alloc_inode();
    }

    printf("+ mknod create %s [%04o] - #%d\n", path, mode, inum);
    
    char* full = strdup(path);
    char* dir_name = dirname(full);
    int dir_num = tree_lookup(dir_name);
    free(full);
    inode* dir = get_inode(dir_num);
    char* base = basename(strdup(path));

    return directory_put(dir, base, inum);
}

slist*
storage_list(const char* path)
{
    int dir_num = tree_lookup(path);
    inode* dir = get_inode(dir_num);
    return directory_list(dir);
}

int
storage_unlink(const char* path)
{
    char* full = strdup(path);
    char* dir_name = dirname(full);
    int dir_num = tree_lookup(full);
    free(full);
    inode* dir = get_inode(dir_num);
    return directory_delete(dir, path);
}

int
storage_link(const char* from, const char* to)
{
    // Hard link they share the same inode

    int first = tree_lookup(from);
    int second = tree_lookup(to);
    
    if (first < 0) {
        printf("Destination file doesn't exist\n");
        return -ENOENT;
    }
    if (second > 0) {
        printf("Destination must not already exist\n");
        return -1;
    }
    
    char* full = strdup(to);
    char* dir_name = dirname(full);
    int dir_num = tree_lookup(dir_name);
    free(full);

    if (dir_num < 0) {
        printf("Directory path does not exist\n");
        return -ENOENT;
    }
    
    inode* dir = get_inode(dir_num);
    
    return directory_put(dir, to, first);
}

int storage_symlink(const char* from, const char* to) {
    //TODO: Implement symlinks
}

int
storage_rename(const char* from, const char* to)
{
    int link_rv = storage_link(from, to);
    if (link_rv < 0) {
        return link_rv;
    }

    int trash = tree_lookup(from);
    return directory_delete(get_inode(trash), from);
    
}

int
storage_set_time(const char* path, const struct timespec ts[2])
{
    //TODO: Write storage_set_time, right now this does nothing
    // Maybe we need space in a pnode for timestamps.
    return 0;
}
